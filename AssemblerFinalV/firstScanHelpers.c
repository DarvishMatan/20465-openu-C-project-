#include "header.h"

/**
 * This file contains all the first scan assisting functions
 */

/**
 * This function receives a string and verifies that it is a viable label name.
 */
int verifyLabelName(char *labelName) {
	int i;

	if (!*labelName) {/* if the label name is empty */
		addToErrorList("Illegal label name");
		return ERROR;
	}

	if (!isalpha(*labelName)) {
		addToErrorList("Label name must start with an alphabetic character");
		return ERROR;
	}

	if (strlen(labelName) > MAX_LABEL) {/* max length of a label */
		addToErrorList("Label name may be at most 31 characters long");
		return ERROR;
	}

	for (i = 0; i < strlen(labelName); i++) {
		if (!isalpha(labelName[i]) && !isdigit(labelName[i])) {
			/* If there are illegal char in the string */
			addToErrorList("Label contains an illegal character");
			return ERROR;
		}
	}

	labelName[strlen(labelName)] = '\0';

	if (numOfRegister(labelName) != ERROR) {
		addToErrorList("Label names identical to registers are not allowed");
		return ERROR;
	}

	if (identifyCommand(labelName) != ERROR) {
		addToErrorList(
				"Label names identical to operation names are not allowed");
		return ERROR;
	}

	return SUCCESS;
}

/**
 * This functions handles all lines of assembly code that are related to external data
 */
int externalHandler(char *externalName) {
	char temp[MAX_LINE]; /* Will hold each word in the string */
	char name[MAX_LINE]; /* This will hold the name of the symbol */

	/**
	 * Retrieves the symbol of the external
	 */
	getNextStr(externalName, temp);

	if (temp[0] != '\0') {
		strcpy(name, temp); /* this will be the name of the symbol */
	} else {
		addToErrorList("Symbol name missing");
		return ERROR;
	}

	if (verifyLabelName(name) == ERROR)/* if the label name is illegal */
		return ERROR;

	/**
	 * Retrieves the text after the symbol, if there are any.
	 */
	getNextStr(NULL, temp);

	/* if there is a character after the symbol (in extern) */
	if (*temp) {
		addToErrorList("Extraneous text");
		return ERROR;
	}

	if (addToSymbolList(DEF, name, external, no, "") == ERROR)
		return ERROR;

	return SUCCESS;
}

/**
 * This functions handles all lines of assembly code that are related to entry lines
 */
int entryHandler(char *entryName) {
	char temp[MAX_LINE]; /* Will hold each word in the string */
	char name[MAX_LINE]; /* This will hold the name of the symbol */

	/**
	 * Retrieves the symbol of the external
	 */
	getNextStr(entryName, temp);

	if (temp[0] != '\0') {
		strcpy(name, temp); /* this will be the name of the symbol */
	} else {
		addToErrorList("Symbol name missing");
		return ERROR;
	}

	if (verifyLabelName(name) == ERROR)
		return ERROR;

	/**
	 * Retrieves the text after the symbol, if there are any.
	 */
	getNextStr(NULL, temp);

	if (*temp) {
		addToErrorList("Extraneous text");
		return ERROR;
	}

	return SUCCESS;
}

/**
 * This function identifies if the line needs to be sent to dataHandler or stringHandler
 */
int isDataOrString(char *str) {
	if (!(strcmp(str, ".string")) || !(strcmp(str, ".data"))) {
		return SUCCESS;
	} else
		return ERROR;
}

/**
 * this function assumes we get the line past the point where .data ends
 *
 *  EX: " 6,5,-17, 9"
 */
int dataHandler(char *line, int *DC) {
	char temp[MAX_LINE];
	int counter; /* counts how many variables we have found */
	int commaFlag; /* a flag that we use to know if a comma we find is illegal */
	int charFound; /* a flag that we use to know if we are in the middle of deciphering a value */
	int wsEncountered; /* a flag we use to know if there was a whitespace between strings */
	int i;
	int j;

	getNextStr(line, temp);

	if (temp[0] == '\0') {
		addToErrorList("Data is uninitialized");
		return ERROR;
	}

	/*
	 * Passes through the line and converts each of the given number arguments into a
	 * member of the data list. Multiple case flags are used.
	 */
	for (i = 0, j = 0, counter = 0, commaFlag = FALSE, charFound = FALSE;
			i < strlen(line); i++) {
		if (isspace( line[i])) {
			/* we skip over whitespace */
			wsEncountered = TRUE;
			continue;
		} else if (charFound && wsEncountered && line[i] != ',') {
			/* in case a comma is missing */
			addToErrorList("There must be a comma between each number");
			return ERROR;
		} else if (!charFound && line[i] == ',' && counter == 0) {
			/* if we found a comma before the start of the value list then it is illegal */
			addToErrorList("Illegal comma");
			return ERROR;
		} else if (!isdigit(line[i]) && line[i] != '+' && line[i] != '-'
				&& line[i] != ',') {
			addToErrorList(
					"A data value must only whole numbers that begins with either plus or minus");
			return ERROR;
		} else if (commaFlag == 1 && line[i] == ',') {
			addToErrorList("Consecutive commas are not allowed");
			return ERROR;
		} else if (charFound == 1 && line[i] == ',') {
			commaFlag = TRUE;
			charFound = FALSE;
			j = 0;
			counter++;
			addToDataList(atoi(temp), *DC);
			*DC += 1;
		} else {
			temp[j++] = line[i];
			charFound = TRUE;
			wsEncountered = FALSE;
			commaFlag = FALSE;
		}
	}

	if (commaFlag) {
		addToErrorList("The data list cannot be terminated with a comma");
		return ERROR;
	}
	/* Adding the last number */
	addToDataList(atoi(temp), *DC);
	*DC += 1;

	return SUCCESS;
}

/**
 * This function receives the line past the point where .string ends and adds the characters to the data list
 */
int stringHandler(char *line, int *DC) {
	char temp[MAX_LINE];
	int flag = 0; /* we will use this to know if we are in the middle of deducting the string */
	int i;

	getNextStr(line, temp);

	if (temp[0] == '\0') {
		addToErrorList("String is uninitialized");
		return ERROR;
	}

	/**
	 * Passes over the string given as an argument and adds each character into the
	 * data list. Multiple case flags are used.
	 */
	for (i = 0; i < strlen(line); i++) {
		if (isspace(line[i]) && !flag) {
			/* skips over whitespace that isn't inside the string */
			continue;
		}
		if (isspace(line[i]) && flag) {
			addToDataList(line[i], *DC);
			*DC += 1;
		} else if (line[i] == '"' && flag == 0) {
			/* start of the string */
			flag = 1;
		} else if (line[i] == '"' && flag == 1) {
			/* end of the string */
			flag = 2;
			i++;
			break;
		} else if (flag == 0) {
			addToErrorList(
					"A string must be wrapped on both sides with quotation marks");
			return ERROR;
		} else if (flag == 1) {
			addToDataList(line[i], *DC);
			*DC += 1;
		} else if (flag) {
			addToErrorList("String contains illegal character");
			return ERROR;
		} else {
			addToErrorList("Characters must be inside quotation marks");
			return ERROR;
		}
	}
	if (flag != 2) {
		addToErrorList(
				"A string must be wrapped on both sides with quotation marks");
		return ERROR;
	}

	for (; i < strlen(line); i++) {
		if (!isspace(line[i])) {
			addToErrorList("Extraneous text");
			return ERROR;
		}
	}

	addToDataList('\0', *DC);
	*DC += 1;

	return SUCCESS;
}

/**
 * here we checked the immediate encode numbers perfection
 */
int verifyImmediate(char *param) {
	int i;
	i = 1;/* pass over the # */

	if (param[i] == '+' || param[i] == '-') {
		i++;
	}
	for (; i < strlen(param); i++) {
		if (!isdigit(param[i])) {/* checks if each member is proper */
			addToErrorList("Enter legal operands");
			return ERROR;
		}
	}

	return SUCCESS;
}

/**
 * In this function we checks if the operands encoding adressing is right
 * for each instruction
 * we got the number of the action by her name (enum) , the parameter and
 * the space in the sentence (src or dest) and the operand
 */
int checkEncodeAdressing(char *parameter, action_name instruct,
		operandSpace space) {
	int whatAdressing = firstScanIdentifyAdressing(parameter);
	/*the encode of the operand adressing*/

	int j;
	
	if (whatAdressing == ERROR) {/* if the adressing is unknown */
		return ERROR;
	}

	if (whatAdressing == DEF && verifyImmediate(parameter + 1) == ERROR)
		/* if the operand is an immediate operand, check him */
		return ERROR;

	if (space == src) {/*check the match array of the optionals encode adressing*/
		for (j = 1; j < NUM_OF_ADDRESSING_TYPES + 1; j++) { /* Skip the command itself */
			if (operands_srcs[instruct][j] && j - 1 == whatAdressing)
				/* here we checked the perfection of the encode adressing
				 * by using the array values (0 or 1).
				 * j-1 is for the proper index of the array by checking the
				 * given addressing too
				 */
				return SUCCESS;
		}
	} else {
		for (j = 1; j < NUM_OF_ADDRESSING_TYPES + 1; j++) /* Skip the command itself */
		{
			if (operands_dest[instruct][j] && j - 1 == whatAdressing) {
				/* here we checked the perfection of the encode adressing
				 * by using the array values (0 or 1).
				 * j-1 is for the proper index of the array by checking the
				 * given addressing too
				 */
				return SUCCESS;
			}
		}
	}
	addToErrorList("Given addressing for the command is not allowed");
	return ERROR;

}

/**
 *  this function gets the parameters area without spaces and calculate the number to promote by commas or words,
 *  if the number of parameters is illegal, write error and return ERROR
 */
int numberOfSteps(char *operands, int numParam, int instruct) {
	int r = DEF;
	char p[MAX_LABEL];
	char temp[MAX_LABEL];
	char test[MAX_LABEL];

	getNextStr(operands, p);/* for the comma test*/

	if (*p == ',') {/* if the start of the operand is a comma */
		addToErrorList("Illegal comma");
		return ERROR;
	}
	getNextOperand(operands, temp);/* here we have the first operand */
	if ((*temp && numParam == 0) || (!(*temp) && numParam >= 1)) /* if there is no legal number of parameters */
	{
		addToErrorList(
				"Illegal number of parameters for the given instruction");
		return ERROR;
	} else if (!numParam) /* if number of parameters is 0 */
	{
		return DEF; /* return the after checking number of parameters */
	} else if (numParam == 2) {/* in case that the instruction has 2 parameters */
		/* if the operand is illegal to the specific function */
		if (checkEncodeAdressing(temp, instruct, src) == ERROR) {
			return ERROR;
		}
		r += isRegister(temp);
		getNextOperand(NULL, p);/* here we have the second operand */
		if (!(*p)) /* if the line has only one parameter */
		{
			addToErrorList(
					"Illegal number of parameters for the given instruction");
			return ERROR;
		}
		/* if the operand is illegal to the specific function */
		if (checkEncodeAdressing(p, instruct, dest) == ERROR) {
			return ERROR;
		}
		r += isRegister(p);/* if 2 operands are registers, promote in 1 */
		if (!strcmp(p, temp))/* if the 2 operands are the same */
		{
			strcpy(test, strstr(operands, p) + strlen(p));/* get the first operand with the same members */
			getNextStr(strstr(test, p) + strlen(p), temp);/*get the extraneous text*/
		} else {
			getNextStr(strstr(operands, p) + strlen(p), temp);/* here we got the extraneous text */
		}
		if (*temp) {
			addToErrorList("Extraneous text");
			return ERROR;
		}
		if (r == 2) {
			return SUCCESS;/* return 1 in case we have 2 registers operands */
		} else
			return 2;
	} else {/* if the instruction has 1 operand */
		/* if the operand is illegal to the specific function */
		if (checkEncodeAdressing(temp, instruct, dest) == ERROR) {
			return ERROR;
		}
		getNextStr(strstr(operands, p) + strlen(p), temp);/* here we got the extraneous text */
		if (*temp) {
			addToErrorList("Extraneous text");
			return ERROR;
		}
		return SUCCESS;
	}

	return ERROR;
}

/* this function deals with 3 parameters. gets the parameters area without spaces and calculate the number 
 to promote by commas or words, if the number of parameters is illegal, write error and return ERROR
 */
int promoteSpecial(char *line, int numParam) {
	int r = 0, i;
	char p[MAX_LABEL];
	char temp[MAX_LABEL];
	char test[MAX_LABEL];

	getNextStr(line, p);/* for the comma test*/

	if (*p == ',') {/* if the start of the operand is a comma */
		addToErrorList("Illegal comma");
		return ERROR;
	}

	getNextOperand(line, p);/* here we have the first operand */

	if (!(*p) || *p == '(') {/* if the first operand is nothing or just a '(' without a label before */
		addToErrorList("Enter legal number of operands");
		return ERROR;
	}
	/* if the operand is illegal to the specific function */
	if (checkEncodeAdressing(p, jmp, dest) == ERROR) {
		return ERROR;
	}

	strcpy(test, p);/* in test we have the first operand */

	for (i = 0; i < 2; i++) {
		getNextOperand(NULL, p);/* here we get the parameters in the brackets */
		if (p == NULL) {
			addToErrorList("Enter legal number of operands");
			return ERROR;
		}
		r += isRegister(p);
		if (!i)/* if i == 0 */
			strcpy(temp, p);/* save the parameter in temp */
	}
	getNextStr(strstr(line, ")") + 1, temp); /* here we got the extraneous text */
	if (*temp) {/* if there's extraneous text */
		addToErrorList("Extraneous text");
		return ERROR;
	}

	if (r == 2) {
		return 2;/* return 2 instead of 3 */
	}

	return 3;
}

/**
 * this function operate the instruction line and return the value
 * that the IC should be promote in
 */
int promoteIC(char *line) {
	int numParams, instruct, numParamTemp = 1, t;
	char temp[MAX_LINE];
	char p[MAX_LINE];
	char instructName[MAX_LINE];

	getNextStr(line, temp); /* now we got the instruction name */
	strcpy(instructName, temp);
	instruct = identifyCommand(temp); /* here we checked if the word is a llegal command and return the command number */

	if (instruct == ERROR) {/* checks if the instruction is legal */
		addToErrorList("Illegal instruction name.");
		return ERROR;
	}

	numParams = atoi(instruction_mat[instruct][1]); /* here we have the number of parameters of the instruction */

	/*
	 * Here we deal with illegal syntax of special instructions
	 */
	if ((instruct == jmp) || (instruct == bne) || (instruct == jsr)) {
		getNextOperand(strstr(line, temp) + strlen(temp), temp);/* here we got the first operand */
		getNextStr(strstr(line, temp) + strlen(temp), p);/* here we got the after first operand string */
		if ((*p != '(') && (*p)) /* check if the after operand first char is nothing or the open bracket */
		{
			/* if we don't get the expected syntx as in the special cases */
			addToErrorList("Illegal number of parameters");
			return ERROR;
		}
	}
	/* if we got parameters in the special instruction */
	if (((instruct == jmp) || (instruct == bne) || (instruct == jsr))
			&& (verifyBrackets(line))) {
		if (verifyBrackets(line) == ERROR) {/* if the brackets use is illegal */
			addToErrorList("Expected '(' before ')' token");
			return ERROR;
		}
		/* t contains the number of memory words that the line contains */
		t = promoteSpecial(strstr(line, instructName) + strlen(instructName),
				3);
	} else {
		t = numberOfSteps(strstr(line, instructName) + strlen(instructName),
				numParams, instruct);
	}
	/* if an error occured during the promotion function */
	if (t == ERROR) {
		return ERROR;
	}

	numParamTemp += t;/* add the promotion number */

	return numParamTemp;
}

/**
 * This function gets a line which contains a label
 * and deals with all the cases of a label which could be in the first scan
 */
int labelHandler(char *line, int *IC, int *DC) {
	char temp[MAX_LINE]; /* will be used to break apart the line to separate words */
	char name[MAX_LINE];/* will hold the label name */
	int success;
	int tempAdress;
	int tempDC;

	if (checkComma(line) == ERROR) {/* case like multiple comma */
		addToErrorList("Multiple comma");
		return ERROR;
	}

	getNextStr(line, temp); /*name of the label */

	if (*temp == ':') {/* if the name is just ':' */
		addToErrorList("Label name required");
		return ERROR;
	}
	if (temp[strlen(temp) - 1] != ':') {/* if the end of the label word is not ':' */
		addToErrorList("There must be a white space after a label declaration");
		return ERROR;
	}

	strcpy(name, temp);/* copy the name of the label into name */

	name[strlen(name) - 1] = '\0';/* override the ':' */

	if (verifyLabelName(name) == ERROR) {
		return ERROR;
	}

	if (identifyLabel(name) != ERROR)/* check if the label is already declared */
	{
		addToErrorList("Label is already declared");
		return ERROR;
	}

	getNextStr(NULL, temp);/* The after label string */

	if (!(*temp)) {/* if we get nothing after the label, return error */
		addToErrorList("The line is not a valid assembly sentence");
		return ERROR;
	}

	success = identifyCommand(temp); /* if the label is before action */
	if (success != -1) /* if the label is an action */
	{
		tempAdress = *IC;
		*IC += promoteIC(line + isLabel(line));/* send the line without the label (the number of memory words does'nt change because of a label) */
		addToSymbolList(tempAdress, name, internal, yes, "");/* insert into symbol list the address,name,location, action sentence or not and the type */

		return SUCCESS;
	} else if (isDataOrString(temp) != ERROR) {/* set type to the type of var (if it's a declaration) */
		tempDC = *DC;
		if (!(strcmp(temp, DATA)))
			dataHandler(strstr(line, DATA) + strlen(DATA), DC);
		else
			stringHandler(strstr(line, STRING) + strlen(STRING), DC);

		addToSymbolList(tempDC, name, internal, no, temp);/* insert the node as a data line */

		return SUCCESS;
	} else if (!strcmp(temp, ENTRY)) {/* .entry case in a label line */
		printf("\nWarning at line %d: labels in entry lines are ignored.\n",
				lineCounter);
		entryHandler(strstr(line, ENTRY) + strlen(ENTRY));/* send the name of the label to be entry */
		return DEF;
	} else if (!strcmp(temp, EXTERN)) {/* .extern case in a label line */
		printf("\nWarning at line %d: labels in externals lines are ignored.\n",
				lineCounter);
		externalHandler(strstr(line, EXTERN) + strlen(EXTERN)); /* send the name of the label to be extern */
		return DEF;
	}

	else {
		addToErrorList("Enter a legal instruction or command name");
		return ERROR;
	}

	return DEF;
}

/**
 * here we deals with a command line without label
 */
int commandHandler(char line[], int *IC) {
	char temp[MAX_LINE];
	strcpy(temp, line);
	*IC += promoteIC(line);/* send to promote IC function and promote the IC */
	return SUCCESS;
}

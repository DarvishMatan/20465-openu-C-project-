#include "header.h"

/**
 * This file contains all the second scan assisting functions
 */

/**
 * Searches the given label within the symbol table and returns 1 if it is external.
 */
int isExternal(char *label) {
	symbolPtr temp = symbolHead;

	while (temp) {
		if (!strcmp(temp->name, label)) { /* if the name is in the symbol list */
			if (temp->location == external) /* if the label is external */
				return SUCCESS;
			else
				return ERROR;
		}
		temp = temp->next;
	}

	return ERROR;
}

/**
 * Searches the given label within the symbol table and returns the address, if it doesn't exist, returns -1
 */
int getLabelAddress(char *label) {
	symbolPtr temp = symbolHead;

	while (temp) {
		if (!strcmp(temp->name, label))/* if the label is in the symbol list */
			return temp->address;/* return the address of the label */

		temp = temp->next;
	}

	return ERROR;
}

/**
 * This function receives an operand and the addressing type
 *  and encodes the operand and adds it to the encoding list
 *
 *  srcORdest is used to know how to encode the register if the operand is indeed a register, otherwise it wont be used.
 *  src=1
 *  dest=2
 */
void addressingHandler(char *operand, int adressing, int *IC, int srcORdest) {
	char code[BASE], tempstr[BASE];
	int temp;

	memset(code, '\0', strlen(code)); /* resetting the strings */
	memset(tempstr, '\0', strlen(tempstr)); /* resetting the strings */

	if (adressing == 0) {
		/**
		 * Direct addressing
		 */
		convertToBase10ToBase2Strange(atoi(operand + 1), code, 1);
		addToEncodeList(&objectHead, code, *IC);

	} else if (adressing == 1) {
		/**
		 * Label addressing
		 */
		temp = identifyLabel(operand); /* returns 0 if the label is external */

		if (temp) {
			/* if this section is entered then the label is not external
			 * and we appropriately mark the code as relocatable.
			 * */
			convertToBase10ToBase2Strange(temp, code, 3);
			addToEncodeList(&objectHead, code, *IC);
		} else {
			/* if this section is entered then the label is external
			 * and we appropriately mark the code as external.
			 * */
			convertToBase10ToBase2Strange(temp, code, 2);
			addToEncodeList(&objectHead, code, *IC);
			addToEncodeList(&externalHead, operand, *IC);
		}

	} else {
		/**
		 * Register addressing
		 */
		temp = numOfRegister(operand);

		if (srcORdest == 2)
			strcat(code, "......");

		convertToBase10ToBase2Strange(atoi(operand + 1), tempstr, 0);
		strcat(code, tempstr + 8);

		if (srcORdest == 1)
			strcat(code, "......");

		strcat(code, "..");
		addToEncodeList(&objectHead, code, *IC);
	}

	(*IC)++;
}

/**
 * Encodes an entry command line
 */
int entryEncoder(char *line) {
	char temp[MAX_LABEL], temp2[MAX_LABEL];
	int address;

	getNextStr(line, temp2);/* here is the .entry word */
	getNextStr(NULL, temp);/* here we got the label name to be entry */
	getNextStr(NULL, temp2); /* here we got the extraneous text */

	if (temp2[0]) {/* If there is extraneous text */
		addToErrorList("Extraneous text");
		return ERROR;
	} else if (!temp[0]) { /* if the name is nothing */
		addToErrorList("Entry declared without a name");
		return ERROR;
	}

	address = getLabelAddress(temp);

	if (address == ERROR) {
		addToErrorList("Symbol was never defined");
		return ERROR;
	}

	if (isExternal(temp) != ERROR) {
		addToErrorList("Exporting an external symbol is not allowed");
		return ERROR;
	}

	addToEncodeList(&entryHead, temp, address);
	return SUCCESS;
}

/**
 * Creates an encoded instruction line from all given parameters
 */
void encodeInstruction(int param1, int param2, int opcode, int src, int dest,
		int era, int *IC) {
	char code[BASE], save[BASE];

	/**
	 * We craft the code of the instructions by converting each part of it with our
	 * assisting function. We concatenate the results into our final code.
	 * */

	convertToBase10ToBase2Strange(param1, save, 0);
	strcpy(code, save + 12);

	convertToBase10ToBase2Strange(param2, save, 0);
	strcat(code, save + 12);

	convertToBase10ToBase2Strange(opcode, save, 0);
	strcat(code, save + 10);

	convertToBase10ToBase2Strange(src, save, 0);
	strcat(code, save + 12);

	convertToBase10ToBase2Strange(dest, save, 0);
	strcat(code, save + 12);

	convertToBase10ToBase2Strange(era, save, 0);
	strcat(code, save + 12);

	addToEncodeList(&objectHead, code, *IC);
	(*IC) += 1;
}

/**
 * Receives the operands from the instruction line and encodes them
 */
int encodeOperands(char *op1, char *op2, int *IC) {
	char code[BASE], save[BASE];
	int adressing; /* we use this to hold the result of identify addressing */

	if (!op2) {/* if there is just 1 operand */
		adressing = identifyAddressing(op1);
		if (adressing == ERROR)
			return ERROR;
		addressingHandler(op1, adressing, IC, 0);
	} else if (numOfRegister(op1) != ERROR && numOfRegister(op2) != ERROR) {
		/**
		 * We enter this section in case both operands are both register.
		 * We concatenate the values of the registers together as instructed.
		 */
		convertToBase10ToBase2Strange(atoi(op1 + 1), save, 0);
		strcpy(code, save + 8);
		convertToBase10ToBase2Strange(atoi(op2 + 1), save, 0);
		strcat(code, save + 8);
		strcat(code, "..");
		addToEncodeList(&objectHead, code, *IC);
		(*IC) += 1;
	} else {/* if there are 2 operands */
		adressing = identifyAddressing(op1);
		if (adressing == ERROR)
			return ERROR;
		addressingHandler(op1, adressing, IC, 1);

		adressing = identifyAddressing(op2);
		if (adressing == ERROR)
			return ERROR;
		addressingHandler(op2, adressing, IC, 2);
	}

	return SUCCESS;
}

/**
 * this function deals with special instructions like bne,jsr,jmp with parameters
 * */
int specialInstructionHandler(char* line, int *IC) {
	int opcode, destAddressing, paramOneAddressing, paramTwoAddressing;
	char temp[MAX_LABEL], dest[MAX_LABEL], param1[MAX_LABEL], param2[MAX_LABEL];

	memset(temp, '\0', strlen(temp));
	memset(dest, '\0', strlen(dest));/* here we reset all the characters in the string (problems with garbage strings) */
	memset(param1, '\0', strlen(param1));
	memset(param2, '\0', strlen(param2));

	getNextStr(line, temp); /* here we have the instruction name */

	opcode = identifyCommand(temp);/* the name of the command in enum is the opcode of it */
	
	getNextOperand(strstr(line, instruction_mat[opcode][0]) + 3, dest);/* we retrieve the destination addressing */

	if ((destAddressing = identifyAddressing(dest)) == ERROR)
		return ERROR;

	getNextOperand(NULL, param1);/* here we got the first parameter */

	if ((paramOneAddressing = identifyAddressing(param1)) == ERROR)
		return ERROR;

	getNextOperand(NULL, param2);/* here we got the second parameter */

	if ((paramTwoAddressing = identifyAddressing(param2)) == ERROR)
		return ERROR;

	encodeInstruction(paramOneAddressing, paramTwoAddressing, opcode, 0, 2, 0,
			IC);

	addressingHandler(dest, destAddressing, IC, 0);

	encodeOperands(param1, param2, IC);

	return SUCCESS;
}

/**
 * This function gets a line with instruction word, linked list of symbols, linked list of data.
 */
int instructionHandler(char * line, int *IC) {
	int opcode, era = DEF, destEncode = 0, srcEncode = 0, param1, param2,
			numParams;
	char temp[MAX_LABEL], op1[MAX_LABEL], op2[MAX_LABEL];


	getNextStr(line, temp); /* here we have the instruction name */

	opcode = identifyCommand(temp);

	if (((opcode != jmp) || (opcode != bne) || (opcode != jsr))
			&& (verifyBrackets(line) == 0)) {
		param1 = DEF;
		param2 = DEF;
	} else {
		/**
		 * We enter this section if and only if the opcode is either jsr,bne or jmp
		 */

		return specialInstructionHandler(line, IC);
	}

	numParams = atoi(instruction_mat[opcode][1]); /* getting the number of operands that are defined for the command */

	if (!numParams) {
		/**
		 * We enter this section if and only if the opcode is either rts or stop
		 */
		encodeInstruction(param1, param2, opcode, srcEncode, destEncode, era,
				IC);/* discuss if we do a data structure */

		return DEF;
	}

	getNextOperand(strstr(line, instruction_mat[opcode][0]) + 3, op1);

	if ((srcEncode = identifyAddressing(op1)) == ERROR) {
		return ERROR;
	}

	if (numParams == 2) {
		getNextOperand(NULL, op2);

		if ((destEncode = identifyAddressing(op2)) == ERROR) {
			return ERROR;
		}

		encodeInstruction(param1, param2, opcode, srcEncode, destEncode, era,
				IC);

		encodeOperands(op1, op2, IC);
	} else {
		encodeInstruction(param1, param2, opcode, 0, srcEncode, era, IC);

		encodeOperands(op1, NULL, IC);
	}

	return SUCCESS;
}

/**
 * Writes the contents of a given encodeList to a file
 *
 * fileType 1: Object file
 * fileType 2: Externals file
 * fileType 3: Entry file
 */
int fileWriter(char *fileName, encodePtr ptr, int fileType, int IC, int DC) {
	FILE *fd;
	char newName[MAX_LINE];

	if (!ptr && fileType != 1) /* if the list is empty, then there is no need to do anything */
		return SUCCESS;

	strcpy(newName, fileName);

	switch (fileType) {/* here we take care of all the files types and adds the ending to the file name */
	case 1:
		strcat(newName, OBJECT_FILE_EXTENSTION);
		break;
	case 2:
		strcat(newName, EXTERNAL_FILE_EXTENSTION);
		break;
	case 3:
		strcat(newName, ENTRY_FILE_EXTENSTION);
		break;
	}

	if (!(fd = fopen(newName, "w"))) {
		printf("\nFile '%s' could not be opened or created.\n", fileName);

		return DEF;
	}
	switch (fileType) {
	case 1: /* if the file to be written is an object file */
		fprintf(fd, "\t%d\t%d\n", IC, DC);

		while (ptr) {
			if (ptr->address > 1000) {
				fprintf(fd, "%d", ptr->address);
			} else {
				fprintf(fd, "0%d", ptr->address);
			}

			fprintf(fd, "\t%s\n", ptr->code);

			ptr = ptr->next;
		}
		break;
	case 2:/* if the file to be written is extern or entry file */
	case 3:
		while (ptr) {
			fprintf(fd, "%s\t%d\n", ptr->code, ptr->address);

			ptr = ptr->next;
		}
		break;
	}

	return SUCCESS;
}

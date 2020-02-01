#include "header.h"

/**
 * In this file we have all of the assisting functions which we
 * used in the whole program.
 */

/**
 * Returns the next independent string within the given string or null if there are none to be found.
 *
 * Keeps working on the same string that was given if the first param is NULL
 *
 * functions similarly to strtok.
 */
void getNextStr(char * string, char * temp) {
	static char* src; /* The source string */
	int flag = DEF; /* identifier that will tell us if we have found a non-whitespace char */
	static int i;
	int j;

	/* If the given string isn't null then we reset the function */
	if (string != NULL) {
		src = string;
		i = DEF;
	}

	if (i == strlen(src)) {
		temp[0] = '\0';
		return;
	}

	/* finds the first non-whitespace char and creates a string from the following
	 * chars until a whitespace is encountered.
	 */
	for (j = 0, flag = 0; i < strlen(src); i++) {
		if (!flag && !isspace(src[i])) {
			flag = 1;
			temp[j++] = src[i];
		} else if (flag && !isspace(src[i])) {
			temp[j++] = src[i];
		} else if (flag && isspace(src[i])) {
			break;
		}
	}
	temp[j] = '\0'; /* preventing garbage from appearing past the string */
}

/**
 * This function checks if the line is either an empty line or a comment line
 * In addition, this function checks cases like more than maximum line
 */
int checkLine(char *line) {
	int i;

	if (strlen(line) > MAX_INPUT_LINE) {
		addToErrorList("Lines longer than 80 characters are not allowed");
		return ERROR;
	}

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == ';') {
			return SUCCESS;
		} else if (!isspace(line[i])) {/* if we got to a none white space or ; */
			return DEF;
		}
	}

	return SUCCESS;
}

/**
 *  Receives a string assumed to be
 *  one of the predefined registers and returns the number of the register.
 *  EX: "r1"
 */
int numOfRegister(char *param) {
	int i;

	for (i = 0; i < NUM_OF_REGISTERS; i++) {/*move all over the registers array */
		if (!(strcmp(registers[i], param))) {/*if the registers fits the given string*/
			return i;
		}
	}

	return ERROR;
}

/**
 *  Receives a string and checks if it is one of the predefined registers.
 */
int isRegister(char *param) {
	int i;
	for (i = 0; i < NUM_OF_REGISTERS; i++) {/*move all over the registers array */
		if (!(strcmp(registers[i], param))) {/*if the registers fits the given string*/

			return SUCCESS;
		}
	}

	return DEF;
}

/**
 * This function determines if the string is a command word and returns the value
 * of it
 */
int identifyCommand(char *str) {
	if (!strcmp(str, "mov")) {
		return mov;
	} else if (!strcmp(str, "cmp")) {
		return cmp;
	} else if (!strcmp(str, "add")) {
		return add;
	} else if (!strcmp(str, "sub")) {
		return sub;
	} else if (!strcmp(str, "not")) {
		return not;
	} else if (!strcmp(str, "clr")) {
		return clr;
	} else if (!strcmp(str, "lea")) {
		return lea;
	} else if (!strcmp(str, "inc")) {
		return inc;
	} else if (!strcmp(str, "dec")) {
		return dec;
	} else if (!strcmp(str, "jmp")) {
		return jmp;
	} else if (!strcmp(str, "bne")) {
		return bne;
	} else if (!strcmp(str, "red")) {
		return red;
	} else if (!strcmp(str, "prn")) {
		return prn;
	} else if (!strcmp(str, "jsr")) {
		return jsr;
	} else if (!strcmp(str, "rts")) {
		return rts;
	} else if (!strcmp(str, "stop")) {
		return stop;
	} else {/* if the string doesn't fit to any command */
		return ERROR;
	}
}

/**
 * Converts a given decimal int into binary in strange version.
 *
 * shiftBits is used for A,R,E marking on the string
 *
 * shiftBits 1: Absolute encoding.
 * shiftBits 2: External encoding.
 * shiftBits 3: Relocatable encoding.
 * shiftBits Other: no shift will be applied.
 */
void convertToBase10ToBase2Strange(int num, char * result, int shiftBits) {
	char helper[BASE];
	int isNegative;
	int i;
	int j;

	if (num < DEF)
		isNegative = TRUE;/*turn the isNegative flag on */
	else
		isNegative = FALSE;

	/*
	 * Uses modulus and division to create a binary representation of the given number
	 * */
	for (i = 0; i < BASE; i++) {
		if (num % 2)/* if the digit is odd number */
			helper[i] = BASE_2_STRANGE_1; /* replaces the 1 in binary */
		else if (num)
			helper[i] = BASE_2_STRANGE_0;
		else
			helper[i] = 0;

		num /= 2;
	}

	for (i = 0, j = strlen(helper) - 1; i < BASE; i++) {
		if (helper[BASE - i - 1] == 0)
			result[i] = BASE_2_STRANGE_0;
		else
			result[i] = helper[j--];
	}

	if (isNegative) {
		/* Converting a negative numbers binary representation into twos complement */

		/* swapping the bits */
		for (i = 0; i < BASE; i++) {
			if (result[i] == BASE_2_STRANGE_1)
				result[i] = BASE_2_STRANGE_0;
			else
				result[i] = BASE_2_STRANGE_1;
		}

		/* adding a bit */
		for (i = BASE - 1; i > 0; i--) {
			if (result[i] == BASE_2_STRANGE_1)
				result[i] = BASE_2_STRANGE_0;
			else {
				result[i] = BASE_2_STRANGE_1;
				break;
			}
		}
	}

	if (shiftBits == 1) {
		for (i = 0; i < BASE - 2; i++) {
			result[i] = result[i + 2];
		}
		result[BASE - 2] = BASE_2_STRANGE_0;
		result[BASE - 1] = BASE_2_STRANGE_0;
	} else if (shiftBits == 2) {
		for (i = 0; i < BASE - 2; i++) {
			result[i] = result[i + 2];
		}
		result[BASE - 2] = BASE_2_STRANGE_0;
		result[BASE - 1] = BASE_2_STRANGE_1;
	} else if (shiftBits == 3) {
		for (i = 0; i < BASE - 2; i++) {
			result[i] = result[i + 2];
		}
		result[BASE - 2] = BASE_2_STRANGE_1;
		result[BASE - 1] = BASE_2_STRANGE_0;
	}

	result[BASE] = '\0'; /* preventing garbage from appearing past the string */
}

/**
 * this function checks multiple comma. If there, return -1
 */
int checkComma(char *str) {
	int i;

	for (i = 0; i < strlen(str); ++i) {
		if (str[i] == ',' && str[i + 1] == ',')/* if there are 2 commas in sequence */
			return ERROR;
	}

	return DEF;
}

/**
 * this function removes commas from the command in the same time, the function checks for Multiple commas
 */
int removeComma(char *str) {
	int count = DEF, i;

	for (i = 0; str[i]; i++) {
		if (str[i] != ',')
			str[count++] = str[i];/* override if there is a comma */
	}

	str[count] = '\0'; /* preventing garbage from appearing past the string */

	return DEF;
}

/**
 * Searches the given symbol table for a given label and if found, returns the address of the label.
 */
int identifyLabel(char *label) {
	symbolPtr temp = symbolHead;
	while (temp) {/* move all over the linked list and check if the label is already declared */

		if (!strcmp(temp->name, label)) {
			return temp->address; /* return the adress if found */
		}

		temp = temp->next;
	}

	return ERROR;
}

/**
 *  this function checks if the brackets use in the given file is right
 */
int verifyBrackets(char *line) {
	int i, startB = 0, endB = 0;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == ')' && startB < 1)/* here I checked if the finish brackets
		 is before the start bracket */
			return ERROR;

		if (line[i] == '(') {/* here I checked if there's a start bracket */
			startB++;/* promote the start bracket temp */
		} else if (line[i] == ')') {/*here I checked if there's a finish bracket*/
			endB++;
		}
	}

	if (startB != endB)/* if the number of start brackets is'nt the same as the
	 finish bracket */
		return ERROR;

	return startB;
}

/**
 * this function removes all spaces from string
 */
void removeSpaces(char *str) {
	int count = 0, i = 0;

	for (; str[i] != '\0'; ++i)
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
			/* if isspace */
			str[count++] = str[i];/*insert to the new string */

	str[count] = '\0'; /* preventing garbage from appearing past the string */
}

/**
 * This function checks if the line contains a label and returns the after label index
 */
int isLabel(char *line) {
	int i;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == ':')/*the end of the label */
		{
			return i + 1;/* return an index of the after label */
		}
	}

	return DEF;
}

/**
 * Receives an operand and identifies the type of addressing it is
 *
 * Addressing 2 is not tested because it is a special case and is handled by another function.
 */
int identifyAddressing(char *operand) {

	if (!operand) { /* this is in case the operand is null - specifically used for bne, jmp and jsr */
		return DEF;
	}

	if (operand[0] == '#')/* if the operand is immediate */
		return DEF; /* returns the value of immediate addressing */

	if (numOfRegister(operand) != ERROR)/* if the operand is register */
		return 3; /* returns the value of register addressing */

	if (identifyLabel(operand) != ERROR)/* if the operand is label */
		return SUCCESS; /* returns the value of label addressing */

	addToErrorList("The label is undeclared");/* else, add to the errors list */
	return ERROR;
}

/**
 *  here we checked the addressing of an operand in the first scan
 *  and checks cases like illegal label declaration
 */
int firstScanIdentifyAdressing(char *operand) {
	if (!operand) { /* this is in case the operand is null - specifically used for bne, jmp and jsr */
		return DEF;
	}

	if (operand[0] == '#')/* if the operand is immediate */
		return DEF; /* returns the value of immediate addressing */

	if (numOfRegister(operand) != ERROR)/* if the operand is registers */
		return 3; /* returns the value of register addressing */
	if (verifyLabelName(operand) == ERROR)
		return ERROR;
	return SUCCESS;/* return a label encode addressing */
}

/**
 * Receives a string that is assumed to contain operands in the predefined format and returns each and every one of them.
 * Used only by functions related to instruction line parsing.
 */
void getNextOperand(char * str, char * temp) {
	static char* src; /* The source string */
	int flag = 0; /* identifier that will tell us if we have found a non-whitespace char */
	static int i;
	int j;

	/* If the given string isn't null then we reset the function */
	if (str != NULL) {
		src = str;
		i = 0;
	}

	if (i == strlen(src)) {
		temp[0] = '\0';
		return;
	}

	/*
	 * finds the first non-whitespace char and creates a string from the following
	 * chars until a whitespace is encountered. Also takes parentheses
	 * and commas into consideration.
	 */
	for (j = 0, flag = 0; i < strlen(src); i++) {
		if (src[i] == ',' || src[i] == '(' || src[i] == ')') {
			break;
		}
		if (!flag && !isspace(src[i])) {
			flag = 1;
			temp[j++] = src[i];
		} else if (flag && !isspace(src[i])) {
			temp[j++] = src[i];
		} else if (flag && isspace(src[i])) {
			while (src[i] != ',' && i < strlen(src) && src[i] != '(')
				i++;

			break;
		}
	}

	i++;
	temp[j] = '\0'; /* preventing garbage from appearing past the string */
}

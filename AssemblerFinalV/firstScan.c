#include "header.h"

/**
 * In this file we have the first scan calls. we read each line from the input file
 *  and send it to the corresponding functions
 * In the first scan we deal with all the possible errors which could be
 * solved in the first Scan
 */

int firstscan(FILE *input, int *IC, int *DC) {
	char line[MAX_LINE], temp[MAX_LINE];
	symbolPtr symbolTemp;
	dataPtr dataTemp;
	lineCounter = INIT_LINECOUNTER;/* set the line counter to 1 */
	*IC = INIT_IC;/* set the IC to 1 */
	*DC = DEF;/* set the DC to 0 */

	while (fgets(line, MAX_LINE, input)) { /* loops the entire file line by line */
		getNextStr(line, temp); /* Retrieves the first word in the line */
		if (checkLine(line) || input == NULL || checkLine(line) == ERROR) {
			/* if the line is empty, continues onward to the next line */
		} else if (!strcmp(temp, EXTERN)) {
			externalHandler(strstr(line, EXTERN) + strlen(EXTERN)); /* instruction is removed from the string we send to the 																	handler */
		} else if (isLabel(temp)) { /* Checks if the line contains a label */
			labelHandler(line, IC, DC); /* Adds the label to the symbol table */
		} else if (identifyCommand(temp) != ERROR) {
			commandHandler(line, IC);
		} else if (!strcmp(temp, ENTRY)) {
			entryHandler(strstr(line, ENTRY) + strlen(ENTRY)); /* remove instruction is removed from the string we send to the handler */
		} else if (!strcmp(temp, DATA)) {
			dataHandler(strstr(line, DATA) + strlen(DATA), DC); /* remove instruction is removed from the string we send to the handler */
		} else if (!strcmp(temp, STRING)) {
			stringHandler(strstr(line, STRING) + strlen(STRING), DC); /* we remove instruction from the string we send to the handler */
		} else
			addToErrorList("The line is not a valid assembly sentence");
		lineCounter++;
	}

	if (errorHead) {
		/* this section is entered only if an error has occurred. */
		printErrors(1);
		return ERROR;
	}

	symbolTemp = symbolHead;
	dataTemp = dataHead;

	while (symbolTemp) {
		/* If a symbol is attached to an instruction line we add IC into its address */
		if (symbolTemp->isAction == no && symbolTemp->location == internal) {
			symbolTemp->address += *IC;
		}
		symbolTemp = symbolTemp->next;
	}

	while (dataTemp) {
		/* IC is added into the address of all data list members */
		dataTemp->address += *IC;
		dataTemp = dataTemp->next;
	}

	return *IC - INIT_IC; /* The absolute number of code lines is returned */
}
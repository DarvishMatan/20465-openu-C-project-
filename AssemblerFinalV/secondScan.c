#include "header.h"

/**
 * In this file we have the second scan calls. we read each line from the input file
 * and send it to the corresponding functions
 *
 * The second scan assumes that there are no errors in the file because the first scan has handled those cases.
 */

/* we get the file name in order to create fit names of files */
int secondScan(FILE *input, char *fileName, int *IC, int *DC) {
	char line[MAX_LINE], temp[MAX_LINE];
	int hasLabel = DEF;
	dataPtr tempDataPtr;
	lineCounter = INIT_LINECOUNTER;
	*IC = INIT_IC;

	rewind(input);/* set the file pointer to the start of the input file */

	while (fgets(line, MAX_LINE, input)) /* loops the entire file line by line */
	{
		if (strlen(line) > MAX_INPUT_LINE) {
			continue;
		}

		if (checkLine(line)) {/* if the line is empty or is a comment, continues onward to the next line */
			lineCounter++;
			continue;
		}

		/* add comma check and brackets check */
		getNextStr(line, temp);

		hasLabel = isLabel(temp);

		if (hasLabel) { /* if the line contains a label, it is skipped over */
			getNextStr(NULL, temp); /* continue to the after label string */
		}

		if (identifyCommand(temp) != ERROR) {/* If the string is a command */
			instructionHandler(line + hasLabel, IC); /* The line is sent without the label */
		} else if (!(strcmp(temp, ENTRY))) {
			entryEncoder(line + hasLabel);/* the line without the label */
		} else {
			/* We skip over other lines because they were handled during the first scan */
		}
		lineCounter++;
	}

	if (errorHead) {/* we enter this section only if an error has occurred. */
		printErrors(2);
		return ERROR;
	}

	/* encoding the datalist to object file */

	tempDataPtr = dataHead;

	/* converts each value in the data list into base 2 strange binary */
	while (tempDataPtr) {
		convertToBase10ToBase2Strange(tempDataPtr->value, temp, 0);
		temp[14] = '\0';
		addToEncodeList(&objectHead, temp, tempDataPtr->address);

		tempDataPtr = tempDataPtr->next;
	}

	*IC -= INIT_IC; /* the memory words length */

	if (errorHead) {
		/* we enter this section only if an error has occurred. */
		printErrors(2);
		return ERROR;
	}

	bubbleSort(entryHead);/* sort the entry list */
	bubbleSort(externalHead);/* sort the externals list */

	/* write the output files */
	if (!fileWriter(fileName, objectHead, 1, *IC, *DC)
			|| !fileWriter(fileName, externalHead, 2, *IC, *DC)
			|| !fileWriter(fileName, entryHead, 3, *IC, *DC)) {
		/* we enter this section only if an error has occurred during one of the output functions */
		return DEF;
	}

	return SUCCESS;
}

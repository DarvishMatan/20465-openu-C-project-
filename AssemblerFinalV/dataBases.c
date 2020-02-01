#include "header.h"

/**
 *	This file contains all database related functions that will be used by the assembler.
 */

/**
 * Adds a new member to the data list
 */
int addToDataList(int val, int DC) {
	dataPtr p1, p2, newPtr;

	newPtr = (dataPtr) malloc(sizeof(data));

	if (!newPtr) {
		addToErrorList("Memory allocation for the new data has failed");
		return ERROR;
	}

	/* update the new node to the given details */
	newPtr->address = DC;
	newPtr->value = val;

	p1 = dataHead;

	while (p1) {
		p2 = p1;
		p1 = p1->next;
	}

	if (p1 == dataHead) { /* if there are no nodes */
		dataHead = newPtr;/* the head is the first node */
		newPtr->next = p1;
		return SUCCESS;
	} else {
		p2->next = newPtr;/* insert the new node to the end of the linked list */
		newPtr->next = p1;
		return SUCCESS;
	}
}

/**
 * Adds a new member to the list
 */
int addToErrorList(char* errorDescription) {
	errorPtr p1, p2, newPtr;

	newPtr = (errorPtr) malloc(sizeof(errors));

	if (!newPtr) {
		fprintf(stderr,
				"ERROR: Memory allocation for the error list has failed.");
		return ERROR;
	}
	/* update the new node to the given details */
	strcpy(newPtr->error, errorDescription); /* make a node with the required values */
	newPtr->line = lineCounter;

	p1 = errorHead;

	while (p1) {
		p2 = p1;
		p1 = p1->next;
	}

	if (p1 == errorHead) {/* if there are no nodes */
		errorHead = newPtr; /* the head is the first node */
		newPtr->next = p1;
		return SUCCESS;
	} else {
		p2->next = newPtr;/* insert the new node to the end of the linked list */
		newPtr->next = p1;
		return SUCCESS;
	}
}

/**
 * Adds a new member to the list
 */
int addToSymbolList(int adressOfSymbol, char *nameOfSymbol, int location,
		int isAction, char *varType) {
	symbolPtr p1, p2, newPtr;

	newPtr = (symbolPtr) malloc(sizeof(symbol));

	if (!newPtr) {
		addToErrorList("Memory allocation for the new symbol has failed");
		return ERROR;
	}

	/* update the new node to the given details */
	strcpy(newPtr->name, nameOfSymbol); /* make a node with the required values */
	newPtr->address = adressOfSymbol;
	newPtr->location = location;
	newPtr->isAction = isAction;
	strcpy(newPtr->type, varType);

	p1 = symbolHead;

	while (p1) {
		if (!strcmp(p1->name, nameOfSymbol)) /* here I checked if there is already a symbol */
		{
			addToErrorList("Symbol name has already been defined");
			return ERROR;
		}
		p2 = p1;
		p1 = p1->next;
	}

	if (p1 == symbolHead) {/* if there are no nodes */
		symbolHead = newPtr;/* the head is the first node */
		newPtr->next = p1;
		return SUCCESS;
	} else {
		p2->next = newPtr;/* insert the new node to the end of the linked list */
		newPtr->next = p1;
		return SUCCESS;
	}
}

/**
 * Adds a new item to the encode list, will be used to create the *.ob file
 */
int addToEncodeList(encodePtr *hptr, char *code, int address) {
	encodePtr p1, p2, newPtr;

	newPtr = (encodePtr) malloc(sizeof(encode));

	if (!newPtr) {
		fprintf(stderr, "ERROR: Memory allocation has failed.\n"); /* if the memory allocation has failed */
		exit(DEF);
	}

	/* update the new node to the given details */
	strcpy(newPtr->code, code); /* make a node with the required values */
	newPtr->address = address;

	p1 = *hptr;

	while (p1) {
		p2 = p1;
		p1 = p1->next;
	}

	if (p1 == *hptr) {/* if there are no nodes */
		*hptr = newPtr; /* the head is the first node */
		newPtr->next = p1;
		return SUCCESS;
	} else {
		p2->next = newPtr;/* insert the new node to the end of the linked list */
		newPtr->next = p1;
		return SUCCESS;
	}
}

/**
 * Frees all the memory used by the list
 */
void freeDataList() {
	dataPtr p;

	while (dataHead) {
		p = dataHead;
		dataHead = dataHead->next;
		free(p);
	}
}

/**
 * Frees all the memory used by the list
 */
void freeSymbolList() {
	symbolPtr p;

	while (symbolHead) {
		p = symbolHead;
		symbolHead = symbolHead->next;
		free(p);
	}
}

/**
 * Frees all the memory used by the list
 */
void freeErrorList() {
	errorPtr p;

	while (errorHead) {
		p = errorHead;
		errorHead = errorHead->next;
		free(p);
	}
}

/**
 * Frees all the memory used by the list
 */
void freeEncodeList(encodePtr * hptr) {
	encodePtr p;

	while (*hptr) {
		p = *hptr;
		*hptr = (*hptr)->next;
		free(p);/* free each node in the linked list */
	}
}

/**
 * Frees all the memory used by the all the lists in the program
 */
void freeLists() {
	freeDataList();
	freeEncodeList(&objectHead);
	freeEncodeList(&entryHead);
	freeEncodeList(&externalHead);
	freeSymbolList();
	freeErrorList();
}


/**
 * Prints out a list of all errors that have occurred during a specified scan.
 *
 * Scan 1: FirstScan
 * Scan 2: SecondScan
 */
void printErrors(int scan) {
	errorPtr copyHead = errorHead;

	if (scan == 1)
		fprintf(stderr,
				"\n\nThe following errors have occurred during the first scan:\n");
	else if (scan == 2)
		fprintf(stderr,
				"\n\nThe following errors have occurred during the second scan:\n");

	while (copyHead) {
		fprintf(stderr, "\n Error at line %d: %s.\n", copyHead->line,
				copyHead->error);

		copyHead = copyHead->next;
	}
}

/**
 *  This function swaps data of two nodes p1 and p2
 */
void swap(encodePtr p1, encodePtr p2) {
	int temp = p1->address;
	p1->address = p2->address;
	p2->address = temp;
}

/**
 *  Sorts the given encode list
 */
void bubbleSort(encodePtr start) {
	int swapped;
	encodePtr ptr1;
	encodePtr lptr = NULL;

	/* Checking for empty list */
	if (start == NULL)
		return;
	do {
		swapped = 0;
		ptr1 = start;

		while (ptr1->next != lptr) {/* while not NULL */
			if (ptr1->address > ptr1->next->address) {/*If the current node is bigger than the next */
				swap(ptr1, ptr1->next);/* change the nodes details */
				swapped = 1;
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	} while (swapped);
}

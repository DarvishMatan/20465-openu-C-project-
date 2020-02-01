#include <stdio.h>
#include <string.h> /* here we get all the streams we use in our program */
#include <stdlib.h>
#include <ctype.h>

/**
 * This file contains all the definitions and declarations that will be used by the whole program.
 */

/**
 * Macros used by the program
 */
#define INIT_IC 100
#define MAX_LINE 82
#define MAX_LABEL 31
#define NUM_OF_INSTRUCTION 16
#define BASE 14
#define NUM_OF_REGISTERS 8
#define NUM_OF_ADDRESSING_TYPES 4
#define NUM_OF_OPERANDS 2
#define INIT_LINECOUNTER 1
#define SUCCESS 1
#define TRUE 1
#define BASE_2_STRANGE_1 '/'
#define BASE_2_STRANGE_0 '.'
#define FALSE 0
#define DEF 0
#define ERROR -1
#define MAX_INPUT_LINE 80

#define EXTERN ".extern"
#define ENTRY ".entry"
#define DATA ".data"
#define STRING ".string"

#define OBJECT_FILE_EXTENSTION ".ob"
#define EXTERNAL_FILE_EXTENSTION ".ext"
#define ENTRY_FILE_EXTENSTION ".ent"
#define INPUT_FILE_EXTENSTION ".as"



/**
 * Enum that defines all the commands that the assembler recognizes
 */

/* the commands enum with the value of each command */
typedef enum {
	mov = 0,
	add,
	cmp,
	sub,
	lea,
	not,
	clr,
	inc,
	dec,
	jmp,
	bne,
	red,
	prn,
	jsr,
	rts,
	stop
} action_name;

/**
 * The encoding types: A is absolute, R is Relocatable, E is external
 */
typedef enum {
	A, R, E
} linkType;

/**
 * The types of operands
 * source or destination
 */
typedef enum {
	src, dest
} operandSpace;

/**
 * Enum that describes if the symbol is external or internal
 */
typedef enum {
	external, internal
} location;

/**
 * Enum that describes if the symbol is an action line or instruction line
 */
typedef enum {
	yes, no
} action_sentence;

/**
 * Collections of pointers that will be used by the assembler to interact with the databases
 */
typedef struct symbolNode *symbolPtr;
typedef struct dataNode *dataPtr;
typedef struct encodeNode *encodePtr;
typedef struct errorNode *errorPtr;

/**
 * The definition of all the linked lists that will act as the databases
 */

/* here we defined the errors nodes */
typedef struct errorNode {
	int line; /* number of lines */
	char error[MAX_LINE]; /* description of the error */
	errorPtr next;
} errors;

/* here we defined the encode nodes */
typedef struct encodeNode {
	char code[MAX_LABEL]; /* can be either a label or a base 2 strange code */
	int address;
	encodePtr next;
} encode;

/* here we defined the data nodes */
typedef struct dataNode {
	int address;
	int value;
	dataPtr next;
} data;

/* here we defined the symbol nodes */
typedef struct symbolNode {
	char name[MAX_LINE];
	int address;
	location location;/* internal or external */
	action_sentence isAction; /* action line or not */
	char type[MAX_LABEL]; /* the type of the data (string or data) */
	symbolPtr next;
} symbol;

/**
 * Global pointers to data, symbol and error lists
 */
dataPtr dataHead;
symbolPtr symbolHead;
errorPtr errorHead;
encodePtr objectHead;
encodePtr entryHead;
encodePtr externalHead;

/* A collection of global variables that will be defined and explained in assmebler.c */
extern char *registers[NUM_OF_REGISTERS];/* the registers names */
extern char *instruction_mat[NUM_OF_INSTRUCTION][NUM_OF_OPERANDS];/* each command and her number of operands */
extern int operands_srcs[NUM_OF_INSTRUCTION][NUM_OF_ADDRESSING_TYPES + 1]; /* each instruction with the encode addressing of the source operands */
extern int operands_dest[NUM_OF_INSTRUCTION][NUM_OF_ADDRESSING_TYPES + 1];/* each instruction with the encode addressing of the destination operands */
extern int lineCounter;

/**
 * Functions that are contained in dataBases.c that will be used by the whole program
 */
int addToDataList(int val, int DC);
int addToSymbolList(int adressOfSymbol, char *nameOfSymbol, int location,
		int isAction, char *varType);
int addToErrorList(char* errorDescription);
int addToEncodeList(encodePtr *hptr, char *code, int address);
void freeErrorList();
void freeDataList();
void freeSymbolList();
void freeLists();
void printErrors(int scan);
void swap(encodePtr a, encodePtr b);
void bubbleSort(encodePtr start);

/**
 * Functions that are contained in assistingFunctions.c that will be used by the program globally
 */
int isRegister(char *);
void convertToBase10ToBase2Strange(int num, char * result, int shiftBits);
int checkLine(char *line);
void getNextStr(char * string, char * temp);
int identifyCommand(char *str);
int checkComma(char *str);
void getNextOperand(char * str, char * temp);
int removeComma(char *str);
void removeSpaces(char *str);
int verifyBrackets(char *line);
int isLabel(char line[]);
int identifyLabel(char *label);
int numOfRegister(char *param);
int identifyAddressing(char *operand);
int firstScanIdentifyAdressing(char *operand);

/**
 * The scanners that assembler.c will use.
 */
int firstscan(FILE *input, int *IC, int *DC);
int secondScan(FILE *input, char *fileName, int *IC, int *DC);

/**
 * Functions that will be used by firstScan.c
 */
int verifyLabelName(char *name);
int externalHandler(char * line);
int dataHandler(char *line, int *DC);
int stringHandler(char *line, int *DC);
int labelHandler(char *line, int *IC, int *DC);
int commandHandler(char line[], int *IC);
int entryHandler(char *entryName);

/**
 * Functions that will be used by secondScan.c
 */
int entryEncoder(char *line);
int instructionHandler(char * line, int *IC);
int fileWriter(char *fileName, encodePtr ptr, int fileType, int IC, int DC);

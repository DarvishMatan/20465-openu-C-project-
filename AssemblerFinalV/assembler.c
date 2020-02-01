#include "header.h"

/**
 * this file is the instructions book of our program. from the main in this file
 * we call the matching functions and check the status of each scan
 */

/**
 * Definitions of the global variables we declared in the main header:
 *
 * instruction_mat: Used to know how many operands the instruction allows.
 * operands_srcs: Used to know which of the four addressing types we allow for any of the instructions for the source operand.
 * operands_dest: Used to know which of the four addressing types we allow for any of the instructions for the destination operand.
 * registers: Used to know what the name of the seven pre-defined register names are.
 * lineCounter: Used to keep track of which line we are currently located at when we scan a given file.
 */

char *instruction_mat[NUM_OF_INSTRUCTION][NUM_OF_OPERANDS] = { { "mov", "2" }, {
		"add", "2" }, { "cmp", "2" }, { "sub", "2" }, { "lea", "2" }, { "not",
		"1" }, { "clr", "1" }, { "inc", "1" }, { "dec", "1" }, { "jmp", "1" }, {
		"bne", "1" }, { "red", "1" }, { "prn", "1" }, { "jsr", "1" }, { "rts",
		"0" }, { "stop", "0" } };

int operands_srcs[NUM_OF_INSTRUCTION][NUM_OF_ADDRESSING_TYPES + 1] = { { mov, 1,
		1, 0, 1 }, { add, 1, 1, 0, 1 }, { cmp, 1, 1, 0, 1 },
		{ sub, 1, 1, 0, 1 }, { lea, 0, 1, 0, 0 }, { not, 0, 0, 0, 0 }, { clr, 0,
				0, 0, 0 }, { inc, 0, 0, 0, 0 }, { dec, 0, 0, 0, 0 }, { jmp, 0,
				0, 0, 0 }, { bne, 0, 0, 0, 0 }, { red, 0, 0, 0, 0 }, { prn, 0,
				0, 0, 0 }, { jsr, 0, 0, 0, 0 }, { rts, 0, 0, 0, 0 }, { stop, 0,
				0, 0, 0 } };

int operands_dest[NUM_OF_INSTRUCTION][NUM_OF_ADDRESSING_TYPES + 1] = { { mov, 0,
		1, 0, 1 }, { add, 0, 1, 0, 1 }, { cmp, 1, 1, 0, 1 },
		{ sub, 0, 1, 0, 1 }, { lea, 0, 1, 0, 1 }, { not, 0, 1, 0, 1 }, { clr, 0,
				1, 0, 1 }, { inc, 0, 1, 0, 1 }, { dec, 0, 1, 0, 1 }, { jmp, 0,
				1, 1, 1 }, { bne, 0, 1, 1, 1 }, { red, 0, 1, 0, 1 }, { prn, 1,
				1, 0, 1 }, { jsr, 0, 1, 1, 1 }, { rts, 0, 0, 0, 0 }, { stop, 0,
				0, 0, 0 } };

char *registers[NUM_OF_REGISTERS] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6",
		"r7" };

int lineCounter;/* here we declared the line counter who knows as a global var in  all the prog*/

int main(int argc, char *argv[]) {
	char str[MAX_LABEL];
	FILE *input; /* file descriptor for the input */
	int IC;
	int DC;
	int status = DEF; /* we will use this to track if the scans were successful */
	int i;

	if (argc < 2) {/*if in first place we don't get a file as input */
		fprintf(stderr, "\nERROR: At least one input file must be given.\n");
		printf("\nTerminating program, farewell.\n");
		return DEF;
	}

	/* move all over the whole files */
	for (i = 1; i < argc; i++) {
		strcpy(str, argv[i]);
		strcat(str, INPUT_FILE_EXTENSTION);/* we got only the name of the file.
		 that's why we need to concatenate the ending of the file (.as)*/
		if (!(input = fopen(str, "r"))) {
			fprintf(stderr,
					"\nERROR: File '%s' could not be accessed. Moving on to the next file.\n",
					argv[i]);
			continue;
		}

		printf("\nAssembler has started working on file '%s'.\n", str);
		IC = INIT_IC;
		DC = DEF;

		status = firstscan(input, &IC, &DC);

		if (status == ERROR) {/* If errors happened during first scan */
			fprintf(stderr,
					"\nErrors encountered during first scan. Moving on to the next file.\n");
			freeLists();
			continue;
		}

		status = secondScan(input, argv[i], &IC, &DC);
		if (status == ERROR) {/* If errors happened during second scan */
			fprintf(stderr,
					"\nErrors encountered during second scan. Moving on to the next file.\n");
			freeLists();
			continue;
		}

		printf("\nAssembler has successfully finished its work on file '%s'.\n",
				str);
		freeLists();/* free the lists */

	}

	printf(
			"\nAssembler has finished scanning all given files.\n\nTerminating program, farewell.\n");
	return DEF;
}


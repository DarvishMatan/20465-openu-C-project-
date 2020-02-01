assembler : assembler.o assistingFunctions.o dataBases.o firstScan.o firstScanHelpers.o secondScan.o secondScanHelpers.o
	gcc -g -ansi -pedantic -Wall assembler.o assistingFunctions.o dataBases.o firstScan.o firstScanHelpers.o 		secondScan.o secondScanHelpers.o -o assembler
assembler.o : assembler.c header.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o
assistingFunctions.o : assistingFunctions.c header.h
	gcc -c -ansi -pedantic -Wall assistingFunctions.c -o assistingFunctions.o
dataBases.o : dataBases.c header.h
	gcc -c -ansi -pedantic -Wall dataBases.c -o dataBases.o
firstScan.o : firstScan.c header.h
	gcc -c -ansi -pedantic -Wall firstScan.c -o firstScan.o
firstScanHelpers.o : firstScanHelpers.c header.h
	gcc -c -ansi -pedantic -Wall firstScanHelpers.c -o firstScanHelpers.o
secondScan.o : secondScan.c header.h
	gcc -c -ansi -pedantic -Wall secondScan.c -o secondScan.o
secondScanHelpers.o : secondScanHelpers.c header.h
	gcc -c -ansi -pedantic -Wall secondScanHelpers.c -o secondScanHelpers.o

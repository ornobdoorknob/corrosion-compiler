
#include <stdio.h>
#include <stdlib.h>

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

/*
***********************************************************
* Function name: main
* Purpose: Main function
* Author: Tahmid Khan Arnab
* Called functions: mainReader(), mainScanner(), mainParser()
* Parameters: Command line arguments - argc, argv
* Return value: Status
* Algorithm: -
*************************************************************
*/

i32 main(int argc, char** argv) {
	i32 i;
	cor_char option;
	printLogo();
	if (DEBUG) {
		for (i = 0; i < argc; ++i)
			printf("argv[%d] = %s\n", i, argv[i]);
	}
	if (argc < 2) {
		printf("%s%c%s%c%s%c%s", "OPTIONS:\n* [",
			PGM_READER, "] - Reader\n* [",
			PGM_SCANNER, "] - Scanner\n* [",
			PGM_PARSER, "] - Parser\n");
		return EXIT_FAILURE;
	}
	option = argv[1][0];
	switch (option) {
	case PGM_READER:
		printf("%s%c%s", "\n[Option '", PGM_READER, "': Starting READER .....]\n\n");
		mainReader(argc, argv);
		break;
	case PGM_SCANNER:
		printf("%s%c%s", "\n[Option '", PGM_SCANNER, "': Starting SCANNER ....]\n\n");
		mainScanner(argc, argv);
		break;
	case PGM_PARSER:
		printf("%s%c%s", "\n[Option '", PGM_PARSER, "': Starting PARSER .....]\n\n");
		mainParser(argc, argv);
		break;
	default:
		printf("%s%c%s%c%s%c%s", "* OPTIONS:\n- [",
			PGM_READER, "] - Reader\n- [",
			PGM_SCANNER, "] - Scanner\n- [",
			PGM_PARSER, "] - Parser\n");
		break;
	}
	return EXIT_SUCCESS;
}

/*
***********************************************************
* Function name: printLogo
* Purpose: Print Logo
* Author: Tahmid Khan Arnab
* Called functions: -
* Parameters: -
* Return value: (Null)
* Algorithm: -
*************************************************************
*/
cor_void printLogo() {
	printf("\nCorrosion Compiler - Reader, Scanner and Parser\n\n");
	printf(" .----------------. \n");
	printf("|C.--------------. |\n");
	printf("|O|     ______   | |\n");
	printf("|R|   .' ___  |  | |\n");
	printf("|R|  / .'   \\_|  | |\n");
	printf("|O|  | |         | |\n");
	printf("|S|  \\ `.___.'\\  | |\n");
	printf("|I|   `._____.'  | |\n");
	printf("|O|              | |\n");
	printf("|N'--------------' |\n");
	printf(" '----------------' \n");
}

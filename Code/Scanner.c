 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/* #define NDEBUG to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef BUFFER_H_
#include "Reader.h"
#endif

#ifndef SCANNER_H_
#include "Scanner.h"
#endif

/* Global objects - variables */
/* This buffer is used as a repository for string literals. */
extern ReaderPointer stringLiteralTable;	/* String literal table */
i32 line;								/* Current line number of the source code */
extern i32 errorNumber;				/* Defined in platy_st.c - run-time error number */

extern i32 stateType[NUM_STATES];
extern string keywordTable[KWT_SIZE];
extern PTR_ACCFUN finalStateTable[NUM_STATES];
extern i32 transitionTable[NUM_STATES][CHAR_CLASSES];

/* Local(file) global objects - variables */
static ReaderPointer lexemeBuffer;			/* Pointer to temporary lexeme buffer */
static ReaderPointer sourceBuffer;			/* Pointer to input source buffer */

/*
 ************************************************************
 * Intitializes scanner
 *		This function initializes the scanner using defensive programming.
 ***********************************************************
 */

i32 startScanner(ReaderPointer psc_buf) {
	i32 i = 0;
	for (i = 0; i < NUM_TOKENS; i++) {
		scData.scanHistogram[i] = 0;
	}
	/* Basic scanner initialization */
	/* in case the buffer has been read previously  */
	readerRecover(psc_buf);
	readerClear(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS; /*0*/
}

/*
 ************************************************************
 * Process Token
 *		Main function of buffer, responsible to classify a char (or sequence
 *		of chars). In the first part, a specific sequence is detected (reading
 *		from buffer). In the second part, a pattern (defined by Regular Expression)
 *		is recognized and the appropriate function is called (related to final states 
 *		in the Transition Diagram).
 ***********************************************************
 */

Token tokenizer(cor_void) {

	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	cor_char c;	/* input symbol */
	i32 state = 0;		/* initial state of the FSM */
	i32 lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	i32 lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	i32 lexLength;		/* token length */
	i32 i;				/* counter */
	/*
	cor_char newc;			// new char
	*/

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = readerGetChar(sourceBuffer);

		/* ------------------------------------------------------------------------
			Part 1: Implementation of token driven scanner.
			Every token is possessed by its own dedicated code
			-----------------------------------------------------------------------
		*/

		switch (c) {

		/* Cases for spaces */
		case ' ':
		case '\t':
		case '\f':
			break;
		case '\n':
			line++;
			break;

		/* Cases for symbols */
		case ';':
			currentToken.code = EOS_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '(':
			currentToken.code = LPR_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case ')':
			currentToken.code = RPR_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '{':
			currentToken.code = LBR_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '}':
			currentToken.code = RBR_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		/* Case for comments and division */
		case '/':
			if (readerGetChar(sourceBuffer) == '/') {
				do {
					c = readerGetChar(sourceBuffer);
					if (c == CHARSEOF0 || c == CHARSEOF255) {
						readerRetract(sourceBuffer);
					}
				} while (c != '\n' && c != CHARSEOF0 && c != CHARSEOF255);
				currentToken.code = CMT_T;
				line++;
			}
			else {
				currentToken.code = ART_T;
				currentToken.attribute.codeType = OP_DIV;
			}
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		/* Cases for END OF FILE */
		case CHARSEOF0:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_0;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case CHARSEOF255:
			currentToken.code = SEOF_T;
			currentToken.attribute.seofType = SEOF_255;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		/* Cases for arithmetic operators*/
		case '+':
			currentToken.code = ART_T;
			currentToken.attribute.codeType = OP_ADD;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '-':
			currentToken.code = ART_T;
			currentToken.attribute.codeType = OP_SUB;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '*':
			currentToken.code = ART_T;
			currentToken.attribute.codeType = OP_MUL;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		/* Cases for relational operators*/
		case '!':
			c = readerGetChar(sourceBuffer);
			if (c == '='){
				currentToken.code = REL_T;
				currentToken.attribute.codeType = OP_NE;
			}
			else {
				currentToken.code = LOG_T;
				currentToken.attribute.codeType = OP_NOT;
			}
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '=':
			currentToken.code = REL_T;
			currentToken.attribute.codeType = OP_EQ;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '<':
			currentToken.code = REL_T;
			currentToken.attribute.codeType = OP_LT;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '>':
			currentToken.code = REL_T;
			currentToken.attribute.codeType = OP_GT;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
			/* Cases for logical operators */
		case '&':
			c = readerGetChar(sourceBuffer);
			if (c == '&') {
				currentToken.code = LOG_T;
				currentToken.attribute.codeType = OP_AND;
			}
			else {
				currentToken.code = ERR_T;
				
			}
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case '|':
			c = readerGetChar(sourceBuffer);
			if (c == '|') {
				currentToken.code = LOG_T;
				currentToken.attribute.codeType = OP_OR;
			}
			else {
				currentToken.code = ERR_T;
			}
			scData.scanHistogram[currentToken.code]++;
			return currentToken;
		case ':':
			currentToken.code = COL_T;
			scData.scanHistogram[currentToken.code]++;
			return currentToken;

		/* ------------------------------------------------------------------------
			Part 2: Implementation of Finite State Machine (DFA) or Transition Table driven Scanner
			Note: Part 2 must follow Part 1 to catch the illegal symbols
			-----------------------------------------------------------------------
		*/

		default: // general case
			state = nextState(state, c);
			lexStart = readerGetPosRead(sourceBuffer) - 1;
			readerSetMark(sourceBuffer, lexStart);
			int pos = 0;
			while (stateType[state] == NOFS) {
				c = readerGetChar(sourceBuffer);
				state = nextState(state, c);
				pos++;
			}
			if (stateType[state] == FSWR)
				readerRetract(sourceBuffer);
			lexEnd = readerGetPosRead(sourceBuffer);
			lexLength = lexEnd - lexStart;
			lexemeBuffer = readerCreate((i32)lexLength + 2, 0, MODE_FIXED);
			if (!lexemeBuffer) {
				fprintf(stderr, "Scanner error: Can not create buffer\n");
				exit(1);
			}
			readerRestore(sourceBuffer);
			for (i = 0; i < lexLength; i++)
				readerAddChar(lexemeBuffer, readerGetChar(sourceBuffer));
			readerAddChar(lexemeBuffer, READER_TERMINATOR);
			currentToken = (*finalStateTable[state])(readerGetContent(lexemeBuffer, 0));
			readerRestore(lexemeBuffer); //xxx
			return currentToken;
		} // switch

	} //while

} // tokenizer


/*
 ************************************************************
 * Get Next State
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	(*) assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:
	(*) Assertion failed: test, file filename, line linenum.
	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUG is used - see the top of the file.
 ***********************************************************
 */

i32 nextState(i32 state, cor_char c) {
	i32 col;
	i32 next;
	col = nextClass(c);
	next = transitionTable[state][col];
	if (DEBUG)
		printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
	assert(next != FS);
	if (DEBUG)
		if (next == FS) {
			printf("Scanner Error: Illegal state:\n");
			printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
			exit(1);
		}
	return next;
}

/*
 ************************************************************
 * Get Next Token Class
	* Create a function to return the column number in the transition table:
	* Considering an input char c, you can identify the "class".
	* For instance, a letter should return the column for letters, etc.
 ***********************************************************
 */

/* Adjust the logic to return next column in TT */
/*    [A-z],[0-9],    _,    &,   \', SEOF,    #, other
	   L(0), D(1), U(2), M(3), Q(4), E(5), C(6),  O(7) */
i32 nextClass(cor_char c) {
	i32 val = -1;
	switch (c) {
	case CHRCOL2:
		val = 2;
		break;
	case CHRCOL3:
		val = 3;
		break;
	case CHRCOL4:
		val = 4;
		break;
	case CHRCOL5:
		val = 5;
		break;
	case CHRCOL6:
		val = 6;
		break;
	case CHARSEOF0:
	case CHARSEOF255:
		val = 8;
		break;
	default:
		if (isalpha(c))
			val = 0;
		else if (isdigit(c))
			val = 1;
		else
			val = 7;
	}
	return val;
}

 /*
  ************************************************************
  * Acceptance State Function IL
  *		Function responsible to identify IL (integer literals).
  * - It is necessary respect the limit (ex: 2-byte integer in C).
  * - In the case of larger lexemes, error should be returned.
  * - Only first ERR_LEN characters are accepted and eventually,
  *   additional three dots (...) should be put in the output.
  ***********************************************************
  */

Token IV(string lexeme) {
	Token currentToken = { 0 };
	i64 tlong;
	if (lexeme[0] != '\0' && strlen(lexeme) > NUM_LEN) {
		currentToken = (*finalStateTable[ESNR])(lexeme);
	}
	else {
		tlong = atol(lexeme);
		if (tlong >= 0 && tlong <= SHRT_MAX) {
			currentToken.code = INL_T;
			currentToken.attribute.intValue = (i32)tlong;
		}
		else {
			currentToken = (*finalStateTable[ESNR])(lexeme);
		}
	}
	scData.scanHistogram[currentToken.code]++;
	return currentToken;
}


/*
 ************************************************************
 * Acceptance State Function ID
 *		In this function, the pattern for IDs must be recognized.
 *		Since keywords obey the same pattern, is required to test if
 *		the current lexeme matches with KW from language.
 *	- Remember to respect the limit defined for lexemes (VID_LEN) and
 *	  set the lexeme to the corresponding attribute (vidLexeme).
 *    Remember to end each token with the \0.
 *  - Suggestion: Use "strncpy" function.
 ***********************************************************
 */

Token funcID(string lexeme) {
	Token currentToken = { 0 };
	size_t length = strlen(lexeme);
	cor_char lastch = lexeme[length - 1];
	i32 isID = COR_FALSE;
	switch (lastch) {
		case MNID_SUF:
			currentToken.code = MNID_T;
			isID = COR_TRUE;
			break;
		default:
			// Test Keyword
			currentToken = funcKEY(lexeme);
			break;
	}
	if (isID == COR_TRUE) {
		strncpy(currentToken.attribute.idLexeme, lexeme, VID_LEN);
		currentToken.attribute.idLexeme[VID_LEN] = CHARSEOF0;
	}
	scData.scanHistogram[currentToken.code]++;
	return currentToken;
}


/*
************************************************************
 * Acceptance State Function SL
 *		Function responsible to identify SL (string literals).
 * - The lexeme must be stored in the String Literal Table 
 *   (stringLiteralTable). You need to include the literals in 
 *   this structure, using offsets. Remember to include \0 to
 *   separate the lexemes. Remember also to incremente the line.
 ***********************************************************
 */
Token SL(string lexeme) {
	Token currentToken = { 0 };
	i32 i = 0, len = (i32)strlen(lexeme);
	currentToken.attribute.contentString = readerGetPosWrte(stringLiteralTable);
	for (i = 1; i < len - 1; i++) {
		if (lexeme[i] == '\n')
			line++;
		if (!readerAddChar(stringLiteralTable, lexeme[i])) {
			currentToken.code = RTE_T;
			strcpy(currentToken.attribute.errLexeme, "Run Time Error:");
			errorNumber = RTE_CODE;
			return currentToken;
		}
	}
	if (!readerAddChar(stringLiteralTable, CHARSEOF0)) {
		currentToken.code = RTE_T;
		strcpy(currentToken.attribute.errLexeme, "Run Time Error:");
		errorNumber = RTE_CODE;
		return currentToken;
	}
	currentToken.code = STR_T;
	scData.scanHistogram[currentToken.code]++;
	return currentToken;
}


/*
************************************************************
 * This function checks if one specific lexeme is a keyword.
 * - Tip: Remember to use the keywordTable to check the keywords.
 ***********************************************************
 */
Token funcKEY(string lexeme) {
	Token currentToken = { 0 };
	i32 kwindex = -1, j = 0;
	for (j = 0; j < KWT_SIZE; j++)
		if (!strcmp(lexeme, &keywordTable[j][0]))
			kwindex = j;
	if (kwindex != -1) {
		currentToken.code = KW_T;
		currentToken.attribute.keywordIndex = kwindex;
	}
	else {
		currentToken.code = VID_T;
	}
	scData.scanHistogram[currentToken.code]++;
	return currentToken;
}

/*
************************************************************
 * Acceptance State Function Error
 *		Function responsible to deal with ERR token.
 * - This function uses the errLexeme, respecting the limit given
 *   by ERR_LEN. If necessary, use three dots (...) to use the
 *   limit defined. The error lexeme contains line terminators,
 *   so remember to increment line.
 ***********************************************************
 */
Token funcErr(string lexeme) {
	Token currentToken = { 0 };
	i32 i = 0, len = (i32)strlen(lexeme);
	if (len > ERR_LEN) {
		strncpy(currentToken.attribute.errLexeme, lexeme, ERR_LEN - 3);
		currentToken.attribute.errLexeme[ERR_LEN - 3] = CHARSEOF0;
		strcat(currentToken.attribute.errLexeme, "...");
	}
	else {
		strcpy(currentToken.attribute.errLexeme, lexeme);
	}
	for (i = 0; i < len; i++)
		if (lexeme[i] == '\n')
			line++;
	currentToken.code = ERR_T;
	scData.scanHistogram[currentToken.code]++;
	return currentToken;
}


/*
 ************************************************************
 * The function prints the token returned by the scanner
 ***********************************************************
 */

cor_void printToken(Token t) {
	extern string keywordTable[]; /* link to keyword table in */
	switch (t.code) {
	case INL_T:
		printf("INL_T\t\t%d\n", t.attribute.intValue);
		break;
	case RTE_T:
		printf("RTE_T\t\t%s", t.attribute.errLexeme);
		/* Call here run-time error handling component */
		if (errorNumber) {
			printf("%d", errorNumber);
			exit(errorNumber);
		}
		printf("\n");
		break;
	case ERR_T:
		printf("ERR_T\t\t%s\n", t.attribute.errLexeme);
		break;
	case SEOF_T:
		printf("SEOF_T\t\t%d\t\n", t.attribute.seofType);
		break;
	case MNID_T:
		printf("MNID_T\t\t%s\n", t.attribute.idLexeme);
		break;
	case STR_T:
		printf("STR_T\t\t%d\t ", (i32)t.attribute.codeType);
		printf("%s\n", readerGetContent(stringLiteralTable, (i32)t.attribute.codeType));
		break;
	case LPR_T:
		printf("LPR_T\n");
		break;
	case RPR_T:
		printf("RPR_T\n");
		break;
	case LBR_T:
		printf("LBR_T\n");
		break;
	case RBR_T:
		printf("RBR_T\n");
		break;
	case KW_T:
		printf("KW_T\t\t%s\n", keywordTable[t.attribute.keywordIndex]);
		break;
	case CMT_T:
		printf("CMT_T\n");
		break;
	case EOS_T:
		printf("EOS_T\n");
		break;
	case ART_T:
		switch (t.attribute.codeType) {
		case OP_ADD:
			printf("ART_T\t\t+\n");
			break;
		case OP_SUB:
			printf("ART_T\t\t-\n");
			break;
		case OP_MUL:
			printf("ART_T\t\t*\n");
			break;
		case OP_DIV:
			printf("ART_T\t\t/\n");
			break;
		}
		break;
	case LOG_T:
		switch (t.attribute.codeType) {
		case OP_AND:
			printf("LOG_T\t\t&&\n");
			break;
		case OP_OR:
			printf("LOG_T\t\t||\n");
			break;
		case OP_NOT:
			printf("LOG_T\t\t!\n");
			break;
		}
		break;
	case REL_T:
		switch (t.attribute.codeType) {
		case OP_EQ:
			printf("REL_T\t\t=\n");
			break;
		case OP_NE:
			printf("REL_T\t\t!=\n");
			break;
		case OP_GT:
			printf("REL_T\t\t>\n");
			break;
		case OP_LT:
			printf("REL_T\t\t<\n");
			break;
		}
		break;
	case COL_T:
		printf("COL_T\n");
		break;
	case VID_T:
		printf("VID_T\t\t\n");
		break;
	default:
		printf("Scanner error: invalid token code: %d\n", t.code);
	}
}

/*
 ************************************************************
 * The function prints statistics of tokens
 * Param:
 *	- Scanner 
 * 
 * Return:
 *	- Void (procedure)
 ***********************************************************
 */
cor_void printScannerData(ScannerData scData) {
	i32 i = 0;
	printf("Statistics\n");
	printf("----------------------------------\n");
	for (i = 0; i < NUM_TOKENS; i++) {
		if(scData.scanHistogram[i] > 0)
			printf("TOKEN[%s]=%d\n", tokenStrTable[i], scData.scanHistogram[i]);
	}
	printf("----------------------------------\n");
}
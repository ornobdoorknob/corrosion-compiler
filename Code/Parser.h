
#ifndef PARSER_H_
#define PARSER_H_

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif
#ifndef READER_H_
#include "Reader.h"
#endif
#ifndef SCANNER_H_
#include "Scanner.h"
#endif

/* Global vars */
static Token			lookahead;
extern BufferReader*	stringLiteralTable;
extern i32				line;
extern Token			tokenizer();
extern string			keywordTable[KWT_SIZE];
static i32				syntaxErrorNumber = 0;

#define LANG_WRTE		"_print%"
#define LANG_READ		"_input%"

/* Constants */
enum KEYWORDS {
	NO_ATTR = -1,

	KW_main,

	KW_int,
	KW_string,

	KW_if,
	KW_else,
	KW_do,
	KW_while,

	KW_let,
	KW_fn,
	KW_print,
	KW_input
};

#define NUM_BNF_RULES 15

/* Parser */
typedef struct parserData {
	i32 parsHistogram[NUM_BNF_RULES];	/* Number of BNF Statements */
} ParserData, * pParsData;

/* Number of errors */
i32 numParserErrors;

/* Scanner data */
ParserData psData;

/* Function definitions */
cor_void startParser();
cor_void matchToken(i32, i32);
cor_void syncErrorHandler(i32);
cor_void printError();
cor_void printBNFData(ParserData psData);

/* List of BNF statements */
enum BNF_RULES {
	BNF_error,					
	BNF_comment,									
	BNF_optVarDeclaration,						
	BNF_optionalStatements,							
	BNF_outputStatement,							
	BNF_outputVariableList,
	BNF_inputVariableList,
	BNF_inputStatement,
	BNF_program,									
	BNF_statement,									
	BNF_statements,									
	BNF_statementsPrime,
	BNF_arithmeticStatement,
	BNF_logicalStatement,
	BNF_ifStatement
};

static string BNFStrTable[NUM_BNF_RULES] = {
	"BNF_error",
	"BNF_comment",
	"BNF_optVarDeclaration",
	"BNF_optionalStatements",
	"BNF_outputStatement",
	"BNF_outputVariableList",
	"BNF_inputVariableList",
	"BNF_inputStatement",
	"BNF_program",
	"BNF_statement",
	"BNF_statements",
	"BNF_statementsPrime",
	"BNF_arithmeticStatement",
	"BNF_logicalStatement",
	"BNF_ifElseStatement"
};

cor_void comment();
cor_void optionalStatements();
cor_void outputStatement();
cor_void inputStatement();
cor_void variableDeclaration();
cor_void outputVariableList();
cor_void inputVariableList();
cor_void program();
cor_void statement();
cor_void statements();
cor_void statementsPrime();
cor_void arithmeticStatement();

#endif

/* TO_DO: Adjust the function header */

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef PARSER_H_
#include "Parser.h"
#endif

/* Parser data */
extern ParserData psData; /* BNF statistics */

/*
************************************************************
 * Process Parser
 ***********************************************************
 */

cor_void startParser() {
	i32 i = 0;
	for (i = 0; i < NUM_BNF_RULES; i++) {
		psData.parsHistogram[i] = 0;
	}
	/* Proceed parser */
	lookahead = tokenizer();
	if (lookahead.code != SEOF_T) {
		program();
	}
	matchToken(SEOF_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Source file parsed");
}


/*
 ************************************************************
 * Match Token
 ***********************************************************
 */
cor_void matchToken(i32 tokenCode, i32 tokenAttribute) {
	i32 matchFlag = 1;
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.keywordIndex != tokenAttribute)
			matchFlag = 0;
	default:
		if (lookahead.code != tokenCode)
			matchFlag = 0;
	}
	if (matchFlag && lookahead.code == SEOF_T)
		return;
	if (matchFlag) {
		lookahead = tokenizer();
		if (lookahead.code == ERR_T) {
			printError();
			lookahead = tokenizer();
			syntaxErrorNumber++;
		}
	}
	else
		syncErrorHandler(tokenCode);
}

/*
 ************************************************************
 * Syncronize Error Handler
 ***********************************************************
 */
cor_void syncErrorHandler(i32 syncTokenCode) {
	printError();
	syntaxErrorNumber++;
	while (lookahead.code != syncTokenCode) {
		if (lookahead.code == SEOF_T)
			exit(syntaxErrorNumber);
		lookahead = tokenizer();
	}
	if (lookahead.code != SEOF_T)
		lookahead = tokenizer();
}

/*
 ************************************************************
 * Print Error
 ***********************************************************
 */
cor_void printError() {
	extern numParserErrors;			/* link to number of errors (defined in Parser.h) */
	Token t = lookahead;
	psData.parsHistogram[BNF_error]++;
	printf("%s%s%3d\n", STR_LANGNAME, ": Syntax error:  Line:", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case ERR_T:
		printf("*ERROR*: %s\n", t.attribute.errLexeme);
		break;
	case SEOF_T:
		printf("SEOF_T\t\t%d\t\n", t.attribute.seofType);
		break;
	case MNID_T:
		printf("MNID_T:\t\t%d\t\n", t.attribute.keywordIndex);
		break;
	case STR_T:
		printf("STR_T: %s\n", readerGetContent(stringLiteralTable, t.attribute.contentString));
		break;
	case KW_T:
		printf("KW_T: %d\n", t.attribute.keywordIndex);
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
	case EOS_T:
		printf("NA\n");
		break;
	case CMT_T:
		printf("CMT_T\n");
		break;
	default:
		printf("%s%s%d\n", STR_LANGNAME, ": Scanner error: invalid token code: ", t.code);
		numParserErrors++;
	}
}

/*
 ************************************************************
 * Program statement
 * BNF: <program> -> _fn _main () { <opt_statements> }
 * FIRST(<program>)= { KW_T(_fn) }.
 ***********************************************************
 */
cor_void program() {
	/* Update program statistics */
	psData.parsHistogram[BNF_program]++;
	/* Program code */
	switch (lookahead.code) {
	case CMT_T:
		comment();
	case KW_T:
		if (lookahead.attribute.keywordIndex == KW_fn) {
			matchToken(KW_T, KW_fn);
			matchToken(KW_T, KW_main);
			matchToken(LPR_T, NO_ATTR);
			matchToken(RPR_T, NO_ATTR);
			matchToken(LBR_T, NO_ATTR);
			optionalStatements();
			matchToken(RBR_T, NO_ATTR);
			break;
		}
		else {
			printError();
		}
	case SEOF_T:
		; // Empty
		break;
	default:
		printError();
	}
	printf("%s%s\n", STR_LANGNAME, ": Program parsed");
}

/*
 ************************************************************
 * comment
 * BNF: comment
 * FIRST(<comment>)= {CMT_T}.
 ***********************************************************
 */
cor_void comment() {
	psData.parsHistogram[BNF_comment]++;
	matchToken(CMT_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Comment parsed");
}

/*
 ************************************************************
 * Optional statement
 * BNF: <opt_statements> -> <statements> | ϵ
 * FIRST(<statement>) = { CMT_T, KW_T(_input), KW_T(_print), KW_T(_let), KW_T(_if), VID_T }
 ***********************************************************
 */
cor_void optionalStatements() {
	psData.parsHistogram[BNF_optionalStatements]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		break;
	case KW_T:
		if (lookahead.attribute.keywordIndex == KW_print || lookahead.attribute.keywordIndex == KW_input ||
			lookahead.attribute.keywordIndex == KW_let || lookahead.attribute.keywordIndex == KW_if) {
			statements();
		}
	break;
	case VID_T:
		statements();
		break;
	default:
		; // Empty
	}
	printf("%s%s\n", STR_LANGNAME, ": Optional statements parsed");
}

/*
 ************************************************************
 * Logical statements
 * BNF: logical_statement
 * FIRST(<logical_statement>)= {LPR_T}.
 ***********************************************************
 */
cor_void logicalStatement() {
	psData.parsHistogram[BNF_logicalStatement]++;
	matchToken(LPR_T, NO_ATTR);
	matchToken(VID_T, NO_ATTR);
	matchToken(REL_T, NO_ATTR);
	matchToken(VID_T, NO_ATTR);
	matchToken(RPR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Logical statement parsed");
}

/*
 ************************************************************
 * if_statement
 * BNF: if_statement
 * FIRST(<if_statement>)= { KW_T(_if) }.
 ***********************************************************
 */
cor_void ifStatement() {
	psData.parsHistogram[BNF_ifStatement]++;
	matchToken(KW_T, KW_if);
	logicalStatement();
	matchToken(LBR_T, NO_ATTR);
	statements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(KW_T, KW_else);
	matchToken(LBR_T, NO_ATTR);
	statements();
	matchToken(RBR_T, NO_ATTR);
}

/*
 ************************************************************
 * arithmetic statement
 * BNF: arithmetic_statement
 * FIRST(<arithmetic_statement>)= {VID_T}.
 ***********************************************************
 */
cor_void arithmeticStatement() {
	psData.parsHistogram[BNF_arithmeticStatement]++;
	matchToken(VID_T, NO_ATTR);
	matchToken(REL_T, OP_EQ);
	matchToken(VID_T, NO_ATTR);
	matchToken(ART_T, NO_ATTR);
	matchToken(VID_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Arithmetic statement parsed");
	statements();
}

/*
 ************************************************************
 * Statements
 * BNF: <statements> -> <statement><statementsPrime>
 * FIRST(<statement>) = { CMT_T, KW_T(_input), KW_T(_print), KW_T(_let), KW_T(_if), VID_T }
 ***********************************************************
 */
cor_void statements() {
	psData.parsHistogram[BNF_statements]++;
	statement();
	statementsPrime();
	printf("%s%s\n", STR_LANGNAME, ": Statements parsed");
}

/*
 ************************************************************
 * Statements Prime
 * BNF: <statementsPrime> -> <statement><statementsPrime> | ϵ
 * FIRST(<statement>) = { CMT_T, KW_T(_input), KW_T(_print), KW_T(_let), KW_T(_if), VID_T }
 ***********************************************************
 */
cor_void statementsPrime() {
	psData.parsHistogram[BNF_statementsPrime]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		break;
	case KW_T:
		if (lookahead.attribute.keywordIndex == KW_print || lookahead.attribute.keywordIndex == KW_input ||
			lookahead.attribute.keywordIndex == KW_let || lookahead.attribute.keywordIndex == KW_if) {
			statements();
		}
		break;
	case VID_T:
		arithmeticStatement();
		break;
	default:
		; //empty string
	}
	printf("%s%s\n", STR_LANGNAME, ": Statements prime parsed");
}

/*
 ************************************************************
 * Single statement
 * BNF: <statement> -> <comment> | <input statement> | <output statement> | <variable declaration> | <if else clause> |
 * <arithmetic statement>
 * FIRST(<statement>) = { CMT_T, KW_T(_input), KW_T(_print), KW_T(_let), KW_T(_if), VID_T }
 ***********************************************************
 */
cor_void statement() {
	psData.parsHistogram[BNF_statement]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		break;
	case KW_T:
		switch (lookahead.attribute.keywordIndex) {
		case KW_print:
			outputStatement();
			break;
		case KW_input:
			inputStatement();
			break;
		case KW_let:
			variableDeclaration();
			break;
		case KW_if:
			ifStatement();
			break;
		default:
			printError();
		}
		break;
	case VID_T:
		arithmeticStatement();
		break;
	default:
		printError();
	}
	printf("%s%s\n", STR_LANGNAME, ": Statement parsed");
}

/*
 ************************************************************
 * Output Statement
 * BNF: <output statement> -> _print (<output statementPrime>);
 * FIRST(<output statement>) = { KW_T(_print) }
 ***********************************************************
 */
cor_void outputStatement() {
	psData.parsHistogram[BNF_outputStatement]++;
	matchToken(KW_T, KW_print);
	matchToken(LPR_T, NO_ATTR);
	outputVariableList();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Output statement parsed");
}

/*
 ************************************************************
 * Input Statement
 * BNF: <input statement> -> _input (<input statementPrime>);
 * FIRST(<input statement>) = { KW_T(_input) }
 ***********************************************************
 */
cor_void inputStatement() {
	psData.parsHistogram[BNF_inputStatement]++;
	matchToken(KW_T, KW_input);
	matchToken(LPR_T, NO_ATTR);
	inputVariableList();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Input statement parsed");
}

/*
 ************************************************************
 * Output variable List
 * BNF: <out_opt_variable list> -> <out_variable list> | ϵ
 * FIRST(<out_opt_variable_list>) = { VID_T, STR_T, ϵ }
 ***********************************************************
 */
cor_void outputVariableList() {
	psData.parsHistogram[BNF_outputVariableList]++;
	switch (lookahead.code) {
	case STR_T:
		matchToken(STR_T, NO_ATTR);
		break;
	case VID_T:
		matchToken(VID_T, NO_ATTR);
		break;
	default:
		;
	}
	printf("%s%s\n", STR_LANGNAME, ": Output variable list parsed");
}

/*
 ************************************************************
 * Input variable List
 * BNF: <in_opt_variable list> -> <in_variable list> | ϵ
 * FIRST(<in_opt_variable_list>) = { VID_T, INL_T, ϵ }
 ***********************************************************
 */
cor_void inputVariableList() {
	psData.parsHistogram[BNF_inputVariableList]++;
	switch (lookahead.code) {
	case VID_T:
		matchToken(VID_T, NO_ATTR);
		break;
	default:
		;
	}
	printf("%s%s\n", STR_LANGNAME, ": Input variable list parsed");
}

/*
 ************************************************************
 * Variable declaration
 * BNF: <opt_variable_declaration> -> <variable_declaration> | ϵ
 * FIRST(<variable_declaration>) = { KW_T(_let) }
 ***********************************************************
 */
cor_void variableDeclaration() {
	psData.parsHistogram[BNF_optVarDeclaration]++;
	matchToken(KW_T, KW_let);
	matchToken(VID_T, NO_ATTR);
	matchToken(COL_T, NO_ATTR);
	switch (lookahead.attribute.keywordIndex) {
	case KW_int:
		matchToken(KW_T, KW_int);
		matchToken(REL_T, OP_EQ);
		matchToken(INL_T, NO_ATTR);
		matchToken(EOS_T, NO_ATTR);
		break;
	case KW_string:
		matchToken(KW_T, KW_string);
		matchToken(REL_T, OP_EQ);
		matchToken(STR_T, NO_ATTR);
		matchToken(EOS_T, NO_ATTR);
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Variable declaration parsed");
}

/*
 ************************************************************
 * The function prints statistics of BNF rules
 * Param:
 *	- Parser data
 * Return:
 *	- Void (procedure)
 ***********************************************************
 */
cor_void printBNFData(ParserData psData) {
	/* Print Parser statistics */
	printf("\nStatistics:\n");
	printf("----------------------------------\n");
	int cont = 0;
	for (cont = 0; cont < NUM_BNF_RULES; cont++) {
		if (psData.parsHistogram[cont] > 0)
			printf("%s%s%s%d%s", "Token[", BNFStrTable[cont], "]=", psData.parsHistogram[cont], "\n");
	}
	printf("----------------------------------\n");
}

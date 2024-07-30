
#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef READER_H_
#include "Reader.h"
#endif

#ifndef SCANNER_H_
#define SCANNER_H_

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*#pragma warning(1:4001) */	/*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*/	/* to enforce C89 comments - to make // comments an error */

/* Constants */
#define VID_LEN 20  /* variable identifier length */
#define ERR_LEN 40  /* error message length */
#define NUM_LEN 5   /* maximum number of digits for IL */

#define RTE_CODE 1  /* Value for run-time error */

#define NUM_TOKENS 18

enum TOKENS {
	ERR_T,		/*  0: Error token */
	MNID_T,		/*  1: Method name identifier token (start: _) */
	INL_T,		/*  2: Integer literal token */
	STR_T,		/*  3: String literal token */
	LPR_T,		/*  4: Left parenthesis token */
	RPR_T,		/*  5: Right parenthesis token */
	LBR_T,		/*  6: Left brace token */
	RBR_T,		/*  7: Right brace token */
	KW_T,		/*  8: Keyword token */
	EOS_T,		/*  9: End of statement (semicolon) */
	RTE_T,		/* 10: Run-time error token */
	SEOF_T,		/* 11: Source end-of-file token */
	CMT_T,		/* 12: Comment token */
	ART_T,		/* 13: Arithmetic operator token */
	LOG_T,		/* 14: Logical operator token */
	REL_T,		/* 15: Relational operator token */
	COL_T,		/* 16: Colon token */
	VID_T		/* 17: VID token */
};

static string tokenStrTable[NUM_TOKENS] = {
	"ERR_T",
	"MNID_T",
	"INL_T",
	"STR_T",
	"LPR_T",
	"RPR_T",
	"LBR_T",
	"RBR_T",
	"KW_T",
	"EOS_T",
	"RTE_T",
	"SEOF_T",
	"CMT_T",
	"ART_T",
	"LOG_T",
	"REL_T",
	"COL_T",
	"VID_T"
};

typedef enum ArithmeticOperators { OP_ADD, OP_SUB, OP_MUL, OP_DIV } AriOperator;
typedef enum RelationalOperators { OP_EQ, OP_NE, OP_GT, OP_LT } RelOperator;
typedef enum LogicalOperators { OP_AND, OP_OR, OP_NOT } LogOperator;
typedef enum SourceEndOfFile { SEOF_0, SEOF_255 } EofOperator;

typedef union TokenAttribute {
	i32 codeType;      /* integer attributes accessor */
	AriOperator arithmeticOperator;		/* arithmetic operator attribute code */
	RelOperator relationalOperator;		/* relational operator attribute code */
	LogOperator logicalOperator;		/* logical operator attribute code */
	EofOperator seofType;				/* source-end-of-file attribute code */
	i32 intValue;						/* integer literal attribute (value) */
	i32 keywordIndex;					/* keyword index in the keyword table */
	i32 contentString;				/* string literal offset from the beginning of the string literal buffer (stringLiteralTable->content) */
	cor_char idLexeme[VID_LEN + 1];		/* variable identifier token attribute */
	cor_char errLexeme[ERR_LEN + 1];		/* error token attribite */
} TokenAttribute;

typedef struct idAttibutes {
	byte flags;			/* Flags information */
	union {
		i32 intValue;				/* Integer value */
		string stringContent;		/* String value */
	} values;
} IdAttibutes;

/* Token declaration */
typedef struct Token {
	i32 code;					/* token code */
	TokenAttribute attribute;	/* token attribute */
	IdAttibutes   idAttribute;	/* not used in this scanner implementation - for further use */
} Token;

/* Scanner */
typedef struct scannerData {
	i32 scanHistogram[NUM_TOKENS];	/* Statistics of chars */
} ScannerData, * pScanData;

///////////////////////////////////////////////////////////////////////////////////////////////////////

/* EOF definitions */
#define CHARSEOF0 '\0'
#define CHARSEOF255 0xFF

/*  Special case tokens processed separately one by one in the token-driven part of the scanner:
 *  LPR_T, RPR_T, LBR_T, RBR_T, EOS_T, SEOF_T and special chars used for tokenis include _, & and ' */
/* These constants will be used on nextClass */
#define CHRCOL2 '_'
#define CHRCOL3 '#'
#define CHRCOL4 '"'
#define CHRCOL5 '%'
#define CHRCOL6 '\n'

/* These constants will be used on VID / MID function */
#define MNID_SUF '%'

#define ESNR	6		/* Error state with no retract */
#define ESWR	7		/* Error state with retract */
#define FS		14		/* Illegal state */

#define NUM_STATES		14
#define CHAR_CLASSES	9

static i32 transitionTable[NUM_STATES][CHAR_CLASSES] = {
/*	[A-z],	[0-9],	_,		#,		",		%,		\n,     other,	\0
	L(0),	D(1),	U(2),	H(3),	Q(4),	M(5),	T(6),	O(7),	F(8)*/
	{3,		8,		5,		1,		10,		1,		ESNR,	ESNR,	ESWR},	// S0: NOAS
	{1,		1,		1,		1,		1,		1,		2,		1,		ESWR},	// S1: NOAS
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S2: FS, SLC
	{3,		4,		4,		4,		4,		4,		4,		4,		ESWR},	// S3: NOAS
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S4: FS, VID
	{5,		5,		5,		7,		7,		6,		7,		7,		9},		// S5: NOAS
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S6: FS, MNID
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S7: FS, KEY
	{9,		8,		9,		9,		9,		9,		9,		9,		ESWR},	// S8: NOAS
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S9: FS, IV
	{10,	10,		10,		10,		11,		10,		10,		10,		ESWR},	// S10: NOAS
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S11: FS, SL
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S12: FS, ES
	{FS,	FS,		FS,		FS,		FS,		FS,		FS,		FS,		FS},	// S13: FS, ER
};

/* Define accepting states types */
#define NOFS	0		/* not accepting state */
#define FSNR	1		/* accepting state with no retract */
#define FSWR	2		/* accepting state with retract */

static i32 stateType[NUM_STATES] = {
	NOFS, /* 00 */
	NOFS, /* 01 */
	FSNR, /* 02 */
	NOFS, /* 03 */
	FSWR, /* 04 */
	NOFS, /* 05 */
	FSNR, /* 06 */
	FSWR, /* 07 */
	NOFS, /* 08 */
	FSWR, /* 09 */
	NOFS, /* 10 */
	FSNR, /* 11 */
	FSNR, /* 12 Err2 - no retract */
	FSWR  /* 13 Err1 - retract */
};

/* Static (local) function  prototypes */
i32			startScanner(ReaderPointer psc_buf);
static i32	nextClass(cor_char c);					/* character class function */
static i32	nextState(i32, cor_char);		/* state machine function */
cor_void	printScannerData(ScannerData scData);

/*
-------------------------------------------------
Automata definitions
-------------------------------------------------
*/
typedef Token(*PTR_ACCFUN)(string lexeme);

/* Declare accepting states functions */
Token funcID  (string lexeme);
Token IV	(string lexeme);
Token SL	(string lexeme);
Token funcErr(string lexeme);
Token funcKEY(string lexeme);

/* 
 * Accepting function (action) callback table (array) definition 
 * If you do not want to use the typedef, the equvalent declaration is:
 */
static PTR_ACCFUN finalStateTable[NUM_STATES] = {
	NULL,		/* -	[00] */
	NULL,		/* -	[01] */
	NULL,		/* Comments, handled in tokenizer	[02] */
	NULL,		/* -	[03] */
	funcID,		/* VID	[04] */
	NULL,		/* -	[05] */
	funcID,		/* MNID [06] */
	funcKEY,	/* KEY	[07] */
	NULL,	    /* -	[08] */
	IV,			/* IV	[09] */
	NULL,	    /* -	[10] */
	SL,			/* SL	[11] */
	funcErr,	/* Err1	[12] */
	funcErr		/* Err2	[13] */
};

/*
-------------------------------------------------
Language keywords
-------------------------------------------------
*/
#define KWT_SIZE 11

static string keywordTable[KWT_SIZE] = {
	"_main",	/* KW00 */

	"_i32",		/* KW01 */
	"_str",		/* KW02 */

	"_if",		/* KW03 */
	"_else",	/* KW04 */
	"_do",		/* KW05 */
	"_while",	/* KW06 */

	"_let",		/* KW07 */
	"_fn",		/* KW08 */
	"_print",	/* KW09 */
	"_input"	/* KW10 */
};

/* NEW SECTION: About indentation */

/*
 * Scanner attributes to be used (ex: including: intendation data
 */

typedef struct languageAttributes {
	cor_char indentationCharType;
	i32 indentationCurrentPos;
} LanguageAttributes;

/* Number of errors */
i32 numScannerErrors;

/* Scanner data */
ScannerData scData;

#endif

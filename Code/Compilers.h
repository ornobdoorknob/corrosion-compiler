
#ifndef COMPILERS_H_
#define COMPILERS_H_

#define DEBUG 0

/* Language name */
#define STR_LANGNAME	"Corrosion"

/* Logical constants*/
#define COR_TRUE  1
#define COR_FALSE 0

/*
------------------------------------------------------------
Data types definitions
------------------------------------------------------------
*/

typedef char			cor_char;
typedef char*			string;
typedef int				i32;
typedef float			f32;
typedef void			cor_void;

typedef unsigned char	boolean;
typedef unsigned char	byte;

typedef long			i64;
typedef double			f64;

/*
------------------------------------------------------------
Programs:
1: Reader - invokes MainReader code
2: Scanner - invokes MainScanner code
3: Parser - invokes MainParser code
------------------------------------------------------------
*/
enum PROGRAMS {
	PGM_READER	= 'R',
	PGM_SCANNER = 'S',
	PGM_PARSER	= 'P'
};

/*
------------------------------------------------------------
Main functions signatures
------------------------------------------------------------
*/
i32 mainReader(i32 argc, string* argv);
i32 mainScanner(i32 argc, string* argv);
i32 mainParser(i32 argc, string* argv);
cor_void printLogo();

#endif

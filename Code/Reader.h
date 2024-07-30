
#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef READER_H_
#define READER_H_


/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* CONSTANTS DEFINITION: GENERAL (NOT LANGUAGE DEPENDENT) .................................. */

/* Modes (used to create buffer reader) */
enum READER_MODE {
	MODE_FIXED = 'F', /* Fixed mode (constant size) */
	MODE_ADDIT = 'A', /* Additive mode (constant increment to be added) */
	MODE_MULTI = 'M'  /* Multiplicative mode (constant increment to be multiplied) */
};

/* Constants about controls (not need to change) */
#define READER_ERROR		(-1)						/* General error message */
#define READER_TERMINATOR	'\0'							/* General EOF */

/* CONSTANTS DEFINITION: PREFIXED BY LANGUAGE NAME .................................. */

/* You should add your own constant definitions here */
#define READER_MAX_SIZE	INT_MAX-1	/* maximum capacity */ 

#define READER_DEFAULT_SIZE			250		/* default initial buffer reader capacity */
#define READER_DEFAULT_INCREMENT	10		/* default increment factor */

/* Add your bit-masks constant definitions here - Defined for BOA */
/* BITS                                (7654.3210) */
#define READER_DEFAULT_FLAG 0x00 	/* (0000.0000)_2 = (000)_10 */
/* TO_DO: BIT 3: FUL = Full */
#define READER_FLAG_FUL 0b00001000  /* Binary representation of 0x08 */
/* TO_DO: BIT 2: EMP: Empty */
#define READER_FLAG_EMP 0b00000100  /* Binary representation of 0x04 */
/* TO_DO: BIT 1: REL = Relocation */
#define READER_FLAG_REL 0b00000010  /* Binary representation of 0x02 */
/* TO_DO: BIT 0: END = EndOfBuffer */
#define READER_FLAG_END 0b00000001  /* Binary representation of 0x01 */

#define NCHAR				128			/* Chars from 0 to 127 */

#define CHARSEOF			(-1)		/* EOF Code for Reader */

/* STRUCTURES DEFINITION: SUFIXED BY LANGUAGE NAME (SOFIA) .................................. */

/* TODO: Adjust datatypes */

/* Offset declaration */
typedef struct offset {
	i32 mark;			/* the offset to the mark position (in chars) */
	i32 read;			/* the offset to the get a char position (in chars) */
	i32 wrte;			/* the offset to the add chars (in chars) */
} Offset;

/* Buffer structure */
typedef struct bufferReader {
	string	content;			/* pointer to the beginning of character array (character buffer) */
	i32	size;				/* current dynamic memory size (in bytes) allocated to character buffer */
	i32	increment;			/* character array increment factor */
	i32	mode;				/* operational mode indicator */
	byte	flags;				/* contains character array reallocation flag and end-of-buffer flag */
	Offset		offset;				/* Offset / position field */
	i32	histogram[NCHAR];	/* Statistics of chars */
	i32	numReaderErrors;	/* Number of errors from Reader */
} BufferReader, * ReaderPointer;

/* FUNCTIONS DECLARATION:  .................................. */
/* General Operations */
ReaderPointer	readerCreate		(i32, i32, i32);
ReaderPointer	readerAddChar		(ReaderPointer const, cor_char);
boolean		readerClear		    (ReaderPointer const);
boolean		readerFree		    (ReaderPointer const);
boolean		readerIsFull		(ReaderPointer const);
boolean		readerIsEmpty		(ReaderPointer const);
boolean		readerSetMark		(ReaderPointer const, i32);
i32		readerPrint		    (ReaderPointer const);
i32		readerLoad			(ReaderPointer const, FILE* const);
boolean		readerRecover		(ReaderPointer const);
boolean		readerRetract		(ReaderPointer const);
boolean		readerRestore		(ReaderPointer const);
/* Getters */
cor_char		readerGetChar		(ReaderPointer const);
string		readerGetContent	(ReaderPointer const, i32);
i32		readerGetPosRead	(ReaderPointer const);
i32		readerGetPosWrte	(ReaderPointer const);
i32		readerGetPosMark	(ReaderPointer const);
i32		readerGetSize		(ReaderPointer const);
i32		readerGetInc		(ReaderPointer const);
i32		readerGetMode		(ReaderPointer const);
byte		readerGetFlags		(ReaderPointer const);
cor_void		readerPrintStat		(ReaderPointer const);
i32		readerNumErrors		(ReaderPointer const);

#endif

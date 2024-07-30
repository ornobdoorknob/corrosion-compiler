
/*
 *.............................................................................
 * MAIN ADVICE:
 * - Please check the "TODO" labels to develop your activity.
 * - Review the functions to use "Defensive Programming".
 *.............................................................................
 */

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef READER_H_
#include "Reader.h"
#endif

/*
***********************************************************
* Function name: readerCreate
* Purpose: Creates the buffer reader according to capacity, increment
	factor and operational mode ('f', 'a', 'm')
* Author: Svillen Ranev / Paulo Sousa
* Called functions: calloc(), malloc()
* Parameters:
*   size = initial capacity
*   increment = increment factor
*   mode = operational mode
* Return value: bPointer (pointer to reader)
* Algorithm: Allocation of memory according to inicial (default) values.
*************************************************************
*/

ReaderPointer readerCreate(i32 size, i32 increment, i32 mode) {
	ReaderPointer readerPointer;

	if (!size) {
		size = READER_DEFAULT_SIZE;
		increment = READER_DEFAULT_INCREMENT;
	}
	if (!increment) {
		increment = READER_DEFAULT_INCREMENT;
		mode = MODE_FIXED;
	}
	if (mode != 'F' && mode != 'A' && mode != 'M') {
		return NULL;
	}
	readerPointer = (ReaderPointer)calloc(1, sizeof(BufferReader));
	if (!readerPointer)
		return NULL;
	readerPointer->content = (string)malloc(size);

	if (!readerPointer->content) {
		return NULL;
	}

	for (int i = 0; i < NCHAR; i++)
		readerPointer->histogram[i] = 0;

	readerPointer->size = size;
	readerPointer->increment = increment;
	readerPointer->mode = mode;
	readerPointer->flags=READER_DEFAULT_FLAG;

	return readerPointer;
}

/*
***********************************************************
* Function name: readerAddChar
* Purpose: Adds a char to buffer reader
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   ch = char to be added
* Return value:
*	readerPointer (pointer to Buffer Reader)
*************************************************************
*/

ReaderPointer readerAddChar(ReaderPointer const readerPointer, cor_char ch) {
	string tempReader = NULL;
	i32 newSize = 0;
	if (!readerPointer) {
		return NULL;
	}
	if (ch > NCHAR || ch < 0) {
		readerPointer->numReaderErrors++;
		return NULL;
	}
	if (readerPointer->offset.wrte * (i32)sizeof(cor_char) < readerPointer->size) {
		readerPointer->content[readerPointer->offset.wrte++]=ch;
		readerPointer->histogram[ch]++;
		return readerPointer;
	} else {
		readerPointer->flags = readerPointer->flags | READER_FLAG_FUL;
		switch (readerPointer->mode) {
		case MODE_FIXED:
			return NULL;
		case MODE_ADDIT:
			newSize = readerPointer->size + readerPointer->increment;
			break;
		case MODE_MULTI:
			newSize = readerPointer->size * readerPointer->increment;
			break;
		default:
			readerPointer->numReaderErrors++;
			return NULL;
		}
		if (newSize < 0) {
			readerPointer->numReaderErrors++;
			return NULL;
		}
		tempReader = (string)realloc(readerPointer->content, newSize);
		if (!tempReader) {
			readerPointer->numReaderErrors++;
			return NULL;
		}
		readerPointer->content = tempReader;
		readerPointer->size = newSize;
	}
	readerPointer->content[readerPointer->offset.wrte++] = ch;
	readerPointer->histogram[ch]++;
	return readerPointer;
}

/*
***********************************************************
* Function name: readerClear
* Purpose: Clears the buffer reader
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerClear(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	readerPointer->offset.mark = 0;
	readerPointer->offset.read = 0;
	readerPointer->offset.wrte = 0;
	for (int i = 0; i < NCHAR; i++) {
		readerPointer->histogram[i] = 0;
	}
	readerPointer->flags = READER_DEFAULT_FLAG;
	return COR_TRUE;
}

/*
***********************************************************
* Function name: readerFree
* Purpose: Releases the buffer address
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerFree(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	free(readerPointer->content);
	free(readerPointer);
	return COR_TRUE;
}

/*
***********************************************************
* Function name: readerIsFull
* Purpose: Checks if buffer reader is full
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerIsFull(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	else if (readerPointer->flags & READER_FLAG_FUL) {
		return COR_TRUE;
	}
	else {
		return COR_FALSE;
	}
}


/*
***********************************************************
* Function name: readerIsEmpty
* Purpose: Checks if buffer reader is empty.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerIsEmpty(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	else if (readerPointer->flags & READER_FLAG_EMP) {
		return COR_TRUE;
	}
	else {
		return COR_FALSE;
	}
}

/*
***********************************************************
* Function name: readerSetMark
* Purpose: Adjust the position of mark in the buffer
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   mark = mark position for char
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerSetMark(ReaderPointer const readerPointer, i32 mark) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	else if (mark > readerPointer->offset.wrte || mark<0) {
		readerPointer->numReaderErrors++;
		return COR_FALSE;
	}
	else {
		readerPointer->offset.mark = mark;
		return COR_TRUE;
	}
}


/*
***********************************************************
* Function name: readerPrint
* Purpose: Prints the string in the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of chars printed.
*************************************************************
*/
i32 readerPrint(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	i32 cont = 0;
	cor_char c;
	c = readerGetChar(readerPointer);
	while (cont < readerPointer->offset.wrte) {
		cont++;
		printf("%c", c);
		c = readerGetChar(readerPointer);
	}
	return cont;
}

/*
***********************************************************
* Function name: readerLoad
* Purpose: Loads the string in the buffer with the content of
	an specific file.
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   fileDescriptor = pointer to file descriptor
* Return value:
*	Number of chars read and put in buffer.
*************************************************************
*/
i32 readerLoad(ReaderPointer const readerPointer, FILE* const fileDescriptor) {
	i32 size = 0;
	cor_char c;
	if (readerPointer == NULL) {
		return READER_ERROR;
	}
	if (fileDescriptor == NULL) {
		readerPointer->numReaderErrors++;
		return READER_ERROR;
	}
	c = (cor_char)fgetc(fileDescriptor);
	while (!feof(fileDescriptor)) {
		if (!readerAddChar(readerPointer, c)) {
			ungetc(c, fileDescriptor);
			return READER_ERROR;
		}
		c = (char)fgetc(fileDescriptor);
		size++;
	}
	return size;
}


/*
***********************************************************
* Function name: readerRecover
* Purpose: Rewinds the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value
*	Boolean value about operation success
*************************************************************
*/
boolean readerRecover(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	readerPointer->offset.read = 0;
	readerPointer->offset.mark = 0;
	return COR_TRUE;
}


/*
***********************************************************
* Function name: readerRetract
* Purpose: Retracts the buffer to put back the char in buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerRetract(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	else if(readerPointer->offset.read < 0){
		readerPointer->numReaderErrors++;
		return COR_FALSE;
	}
	readerPointer->offset.read--;
	return COR_TRUE;
}


/*
***********************************************************
* Function name: readerRestore
* Purpose: Resets the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
boolean readerRestore(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return COR_FALSE;
	}
	readerPointer->offset.read = readerPointer->offset.mark;
	return COR_TRUE;
}


/*
***********************************************************
* Function name: readerGetChar
* Purpose: Returns the char in the getC position.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Char in the getC position.
*************************************************************
*/
cor_char readerGetChar(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	if (readerPointer->offset.read == readerPointer->offset.wrte) {
		readerPointer->flags = readerPointer->flags | READER_FLAG_END;
		return '\0';
	}
	else if(readerPointer->offset.read < readerPointer->offset.wrte){
		readerPointer->flags = readerPointer->flags & ~READER_FLAG_END;
	}
	cor_char c = readerPointer->content[readerPointer->offset.read++];
	return c;
}


/*
***********************************************************
* Function name: readerGetContent
* Purpose: Returns the pointer to String.
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   pos = position to get the pointer
* Return value:
*	Position of string char.
*************************************************************
*/
string readerGetContent(ReaderPointer const readerPointer, i32 pos) {
	if (!readerPointer) {
		return NULL;
	}
	else if (pos < 0 || pos > readerPointer->offset.wrte) {
		readerPointer->numReaderErrors++;
		return NULL;
	}
	return readerPointer->content + pos;
}



/*
***********************************************************
* Function name: readerGetPosRead
* Purpose: Returns the value of getCPosition.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	The read position offset.
*************************************************************
*/
i32 readerGetPosRead(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->offset.read;
}


/*
***********************************************************
* Function name: readerGetPosWrte
* Purpose: Returns the position of char to be added
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Write position
*************************************************************
*/
i32 readerGetPosWrte(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->offset.wrte;
}


/*
***********************************************************
* Function name: readerGetPosMark
* Purpose: Returns the position of mark in the buffer
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Mark position.
*************************************************************
*/
i32 readerGetPosMark(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->offset.mark;
}


/*
***********************************************************
* Function name: readerGetSize
* Purpose: Returns the current buffer capacity
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Size of buffer.
*************************************************************
*/
i32 readerGetSize(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->size;
}

/*
***********************************************************
* Function name: readerGetInc
* Purpose: Returns the buffer increment.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	The Buffer increment.
*************************************************************
*/
i32 readerGetInc(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->increment;
}

/*
***********************************************************
* Function name: readerGetMode
* Purpose: Returns the operational mode
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Operational mode.
*************************************************************
*/
i32 readerGetMode(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->mode;
}


/*
***********************************************************
* Function name: readerGetFlags
* Purpose: Returns the entire flags of Buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Flags from Buffer.
*************************************************************
*/
byte readerGetFlags(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->flags;
}



/*
***********************************************************
* Function name: readerPrintStat
* Purpose: Shows the char statistic.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value: (Void)
*************************************************************
*/
cor_void readerPrintStat(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return;
	}
	for (int i = 1; i < NCHAR; i++) {
		if (readerPointer->histogram[i]>0)
			printf("%c: %d \n", i, readerPointer->histogram[i]);
	}
}

/*
***********************************************************
* Function name: readerNumErrors
* Purpose: Returns the number of errors found.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of errors.
*************************************************************
* OOOOOOOOOOOOOOO i cant increment numReaderErrors without error
*/
i32 readerNumErrors(ReaderPointer const readerPointer) {
	if (!readerPointer) {
		return READER_ERROR;
	}
	return readerPointer->numReaderErrors;
}

/** 
 BeginILUCopyright

 Copyright (c) 1991-1999 Xerox Corporation.  All Rights Reserved.

 Unlimited use, reproduction, modification, and distribution of this
 software and modified versions thereof is permitted.  Permission is
 granted to make derivative works from this software or a modified
 version thereof.  Any copy of this software, a modified version
 thereof, or a derivative work must include both the above copyright
 notice of Xerox Corporation and this paragraph.  Any distribution of
 this software, a modified version thereof, or a derivative work must
 comply with all applicable United States export control laws.  This
 software is made available AS IS, and XEROX CORPORATION DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
 LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
 EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
 NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGES.
  
 EndILUCopyright
*/

/* aprintf works like sprintf, except that instead of using a fixed buffer specified
   as an argument, aprintf dynamically allocates a buffer of the necessary size, and
   returns it as the return result */

#include <stdio.h>
#include <stdarg.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define TRUE		1
#define FALSE		0

typedef struct Buffer_s {
  unsigned int		size;		/* number of chars currently in buffer.mem */
  unsigned int		used;		/* number of chars in buffer.mem that are currently used */
  char *		mem;		/* allocated memory for data */
} * Buffer;

static void AddChar (char the_char, Buffer buffer)
{
  if (buffer->size <= buffer->used)
    {
      buffer->size *= 2;
      buffer->mem = (char *) realloc(buffer->mem, buffer->size);
      if (buffer->mem == NULL)
	{
	  fprintf(stderr, "Can't allocate %d bytes for aprintf buffer!\n", buffer->size);
	  exit(1);
	}
    }
  buffer->mem[buffer->used++] = the_char;
}
     
static void InitBuffer (Buffer buffer, unsigned int size)
{
  buffer->size = size;
  buffer->mem = (char *) malloc(size);
  if (buffer->mem == NULL)
    {
      fprintf(stderr, "Can't allocate %d bytes for aprintf buffer!\n", buffer->size);
      exit(1);
    };
  buffer->used = 0;
}

static int vfprintfInner(Buffer stream, const char *format, va_list args);

char *aprintf (char *fmt, ...)
{
  va_list ap;
  struct Buffer_s buf;
  char *ret;
  int charsPrinted;

  va_start(ap,fmt);

  InitBuffer (&buf, 1000);
  charsPrinted = vfprintfInner (&buf, fmt, ap);
  va_end(ap);
  if (charsPrinted <= 0)
    {
      free (buf.mem);
      return NULL;
    }
  else {
    ret = (char *) realloc(buf.mem, buf.used + 1);
    ret[buf.used] = '\0';
  }
  return (ret);  
}

/*
 ************************************************************************
 *
 * The rest of this file is taken virtually unmodified from the Sprite
 *   stdio implementation ...
 *
 ************************************************************************
 */
 
/* 
 * vfprintf.c --
 *
 *	Source code for the "vfprintf" library procedure.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */
/*
 * The following defines the size of buffer needed to hold the ASCII
 * digits for the largest floating-point number and the largest integer.
 */

#define CVT_DBL_BUF_SIZE 320
#define CVT_INT_BUF_SIZE 33

/*
 *----------------------------------------------------------------------
 *
 * CvtUtoA --
 *
 *	Convert a number from internal form to a sequence of
 *	ASCII digits.
 *
 * Results:
 *	The return value is a pointer to the ASCII digits representing
 *	i, and *lengthPtr will be filled in with the number of digits
 *	stored at the location pointed to by the return value.  The
 *	return value points somewhere inside buf, but not necessarily
 *	to the beginning.  Note:  the digits are left null-terminated.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
CvtUtoA(i, base, buf, lengthPtr, capsFlag)
    register unsigned i;	/* Value to convert. */
    register int base;		/* Base for conversion.  Shouldn't be
				 * larger than 36.  2, 8, and 16
				 * execute fastest.
				 */
    register char *buf;		/* Buffer to use to hold converted string.
				 * Must hold at least CVT_INT_BUF_SIZE bytes. */
    int *lengthPtr;		/* Number of digits is stored here. */
    int capsFlag;		/* 1 if want capital abcd... */
{
    register char *p;
    register char hexOffset = (capsFlag ? 'A': 'a')-'9'-1;

    /*
     * Handle a zero value specially.
     */

    if (i == 0) {
	buf[0] = '0';
	buf[1] = 0;
	*lengthPtr = 1;
	return buf;
    }

    /*
     * Build the string backwards from the end of the result array.
     */

    p = &buf[CVT_INT_BUF_SIZE-1];
    *p = 0;

    switch (base) {

	case 2:
	    while (i != 0) {
		p -= 1;
		*p = '0' + (i & 01);
		i >>= 1;
	    }
	    break;
    
	case 8:
	    while (i != 0) {
		p -= 1;
		*p = '0' + (i & 07);
		i >>= 3;
	    }
	    break;
	
	case 16:
	    while (i !=0) {
		p -= 1;
		*p = '0' + (i & 0xf);
		if (*p > '9') {
		    *p += hexOffset;
		}
		i >>= 4;
	    }
	    break;
	
	default:
	    while (i != 0) {
		p -= 1;
		*p = '0' + (i % base);
		if (*p > '9') {
		    *p += hexOffset;
		}
		i /= base;
	    }
	    break;
    }

    *lengthPtr = (&buf[CVT_INT_BUF_SIZE-1] - p);
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * CvtFtoA --
 *
 *	This procedure converts a double-precision floating-point
 *	number to a string of ASCII digits.
 *
 * Results:
 *	The characters at buf are modified to hold up to numDigits ASCII
 *	characters, followed by a null character.  The digits represent
 *	the most significant numDigits digits of value, with the lowest
 *	digit rounded.  The value at *pointPtr is modified to hold
 *	the number of characters in buf that precede the decimal point.
 *	A negative value of *pointPtr means zeroes must be inserted
 *	between the point and buf[0].  If value is negative, *signPtr
 *	is set to TRUE;	otherwise it is set to FALSE.  The return value
 *	is the number of digits stored in buf, which is either:
 *	(a) numDigits (if the number is so huge that all numDigits places are
 *	    used before getting to the right precision level, or if
 *	    afterPoint is -1)
 *	(b) afterPoint + *pointPtr (the normal case if afterPoint isn't -1)
 *	If there were no significant digits within the specified precision,
 *	then *pointPtr gets set to -afterPoint and 0 is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
CvtFtoA(value, numDigits, afterPoint, pointPtr, signPtr, buf, fpError)
    double value;		/* Value to be converted. */
    int numDigits;		/* Maximum number of significant digits
				 * to generate in result. */
    int afterPoint;		/* Maximum number of digits to generate
				 * after the decimal point.  If -1, then
				 * there there is no limit. */
    int *pointPtr;		/* Will be filled in with position of
				 * decimal point (number of digits before
				 * decimal point). */
    int *signPtr;		/* Modified to indicate whether or not
				 * value was negative. */
    char *buf;			/* Place to store ASCII digits.  Must hold
				 * at least numDigits+1 bytes. */
    int *fpError;               /* pointer to flag that is set if the number
                                   is not a valid number. */

{
    register char *p;
    double fraction, intPart;
    int i, numDigits2;
    char tmpBuf[CVT_DBL_BUF_SIZE];
				/* Large enough to hold largest possible
				 * floating-point number.
				 */

#if 0
/* Can't make these tests in any Standard C / POSIX way, so omit them */

    /*
     * Make sure the value is a valid number
     */
     {
#       ifdef FPCLASS_T
            FPCLASS_T fpclass;
#       endif
  
        if ( ISINF(value,fpclass) ) {
	    /*
	     * Set the error flag so the invoking function will know
	     * that something is wrong.
	     */
	    *fpError = TRUE;
	    strcpy(buf, "Inf");
	    return sizeof("Inf") - 1;
	}
        if ( ISNAN(value,fpclass) ) {
	    *fpError = TRUE;
	    strcpy(buf, "NaN");
	    return sizeof("NaN") - 1;
        }
        *fpError = FALSE;
    }
    /*
     * Take care of the sign.
     */

#endif /* ifdef 0 */

    *fpError = FALSE;

    if (value < 0.0) {
	*signPtr = TRUE;
	value = -value;
    } else {
	*signPtr = FALSE;
    }


    /*
     * Divide value into an integer and a fractional component.  Convert
     * the integer to ASCII in a temporary buffer, then move the characters
     * to the real buffer (since we're converting from the bottom up,
     * we won't know the highest-order digit until last).
     */

    fraction = modf(value, &intPart);
    *pointPtr = 0;
    for (p = &tmpBuf[CVT_DBL_BUF_SIZE-1]; intPart != 0; p -= 1) {
	double tmp;
	char digit;

	tmp = modf(intPart/10.0, &intPart);

	digit = (tmp * 10.0) + .2;
	*p = digit + '0';
	*pointPtr += 1;
    }
    p++;
    for (i = 0; (i <= numDigits) && (p <= &tmpBuf[CVT_DBL_BUF_SIZE-1]);
	    i++, p++) {
	buf[i] = *p;
    }

    /*
     * If the value was zero, put an initial zero in the buffer
     * before the decimal point.
     */
    
    if (value == 0.0) {
	buf[0] = '0';
	i = 1;
	*pointPtr = 1;
    }

    /*
     * Now handle the fractional part that's left.  Repeatedly multiply
     * by 10 to get the next digit.  At the beginning, the value may be
     * very small, so do repeated multiplications until we get to a
     * significant digit.
     */
    
    if ((i == 0) && (fraction > 0)) {
	while (fraction < .1) {
	    fraction *= 10.0;
	    *pointPtr -= 1;
	};
    }

    /*
     * Compute how many total digits we should generate, taking into
     * account both numDigits and afterPoint.  Then generate the digits.
     */
    
    numDigits2 = afterPoint + *pointPtr;
    if ((afterPoint < 0) || (numDigits2 > numDigits)) {
	numDigits2 = numDigits;
    }
    
    for ( ; i <= numDigits2; i++) {
	double tmp;
	char digit;

	fraction = modf(fraction*10.0, &tmp);

	digit = tmp;
	buf[i] = digit + '0';
    }

    /*
     * The code above actually computed one more digit than is really
     * needed.  Use it to round the low-order significant digit, if
     * necessary.  This could cause rounding to propagate all the way
     * back through the number.
     */
    
    if ((numDigits2 >= 0) && (buf[numDigits2] >= '5')) {
	for (i = numDigits2-1; ; i--) {
	    if (i < 0) {
		int j;

		/*
		 * Must slide the entire buffer down one slot to make
		 * room for a leading 1 in the buffer.  Careful: if we've
		 * already got numDigits digits, must drop the last one to
		 * add the 1.
		 */

		for (j = numDigits2; j > 0; j--) {
		    buf[j] = buf[j-1];
		}
		if (numDigits2 < numDigits) {
		    numDigits2++;
		}
		(*pointPtr)++;
		buf[0] = '1';
		break;
	    }

	    buf[i] += 1;
	    if (buf[i] <= '9') {
		break;
	    }
	    buf[i] = '0';
	}
    }

    if (numDigits2 <= 0) {
	numDigits2 = 0;
	*pointPtr = -afterPoint;
    }
    buf[numDigits2] = 0;
    return numDigits2;
}

/*
 *----------------------------------------------------------------------
 *
 * vfprintf --
 *
 *	This utility routine does all of the real work of printing
 *	formatted information.  It is called by printf, fprintf,
 *	sprintf, vprintf, and vsprintf.
 *
 * Results:
 *	The return value is the total number of characters printed.
 *
 * Side effects:
 *	Information is output on stream.  See the manual page entry
 *	for printf for details.
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * vfprintf --
 *
 *	This utility routine does all of the real work of printing
 *	formatted information.  It is called by printf, fprintf,
 *	sprintf, vprintf, and vsprintf.
 *
 * Results:
 *	The return value is the total number of characters printed.
 *
 * Side effects:
 *	Information is output on stream.  See the manual page entry
 *	for printf for details.
 *
 *----------------------------------------------------------------------
 */

static int				/* Number of characters formatted */
vfprintfInner(Buffer stream,		/* Where to output formatted results. */
	      const char *format,	/* Contains literal text and format control
					 * sequences indicating how args are to be
					 * printed.  See the man page for details. */
	      va_list args)		/* Variable number of values to be formatted
					 * and printed. */
{
    int leftAdjust;		/* TRUE means field should be left-adjusted. */
    int minWidth;		/* Minimum width of field. */
    int precision;		/* Precision for field (e.g. digits after
				 * decimal, or string length). */
    int altForm;		/* TRUE means value should be converted to
				 * an alternate form (depends on type of
				 * conversion). */
    register char c;		/* Current character from format string.
				 * Eventually it ends up holding the format
				 * type (e.g. 'd' for decimal). */
    char pad;			/* Pad character. */
    char buf[CVT_DBL_BUF_SIZE+10];
				/* Buffer used to hold converted numbers
				 * before outputting to stream.  Must be
				 * large enough for floating-point number
				 * plus sign plus "E+XXX + null" */
    char expBuf[CVT_INT_BUF_SIZE];
				/* Buffer to use for converting exponents. */
    char *signChar;		/* This is the "+" or " " for the sign pos.  */
    char *prefix;		/* Holds non-numeric stuff that precedes
				 * number, such as "-" or "0x".  This is
				 * kept separate to be sure we add padding
				 * zeroes AFTER the prefix. */
    register char *field;	/* Pointer to converted field. */
    int actualLength;		/* Actual length of converted field. */
    int point;			/* Location of decimal point, for "f" and
				 * "e" conversions. */
    int zeroPad;		/* Zeros to pad number with. */
    int sign;			/* Also used for "f" and "e" conversions. */
    int i, tmp;
    int charsPrinted = 0;	/* Total number of characters output. */
    char *end;
    int fpError = FALSE;

    /*
     * The main loop is to scan through the characters in format.
     * Anything but a '%' is output directly to stream.  A '%'
     * signals the start of a format field;  the formatting information
     * is parsed, the next value from args is formatted and printed,
     * and the loop goes on.
     */

    for (c = *format; c != 0; format++, c = *format) {

	if (c != '%') {
	    AddChar(c, stream);
	    charsPrinted += 1;
	    continue;
	}

	/*
	 * Parse off the format control fields.
	 */

	leftAdjust	= FALSE;
	pad		= ' ';
	minWidth	= 0;
	precision	= -1;
	altForm		= FALSE;
	signChar	= "";
	prefix		= "";
	actualLength	= 0;
	zeroPad		= 0;

	format++;  
	c = *format;
	while (TRUE) {
	    if (c == '-') {
		leftAdjust = TRUE;
	    } else if (c == '0') {
		pad = '0';
	    } else if (c == '#') {
		altForm = TRUE;
	    } else if (c == '+') {
		signChar = "+";
	    } else if (c== ' ') {
		if (!*signChar) {
		    signChar = " ";
		}
	    } else {
		break;
	    }
	    format++;
	    c = *format;
	}
	if (isdigit(c)) {
	    minWidth = strtoul(format, &end, 10);
	    format = end;
	    c = *format;
	} else if (c == '*') {
	    minWidth = va_arg(args, int);
	    format++; 
	    c = *format;
	}
	if (c == '.') {
	    format++; 
	    c = *format;
	    precision = 0;
	}
	if (isdigit(c)) {
	    precision = strtoul(format, &end, 10);
	    format = end;
	    c = *format;
	} else if (c == '*') {
	    precision = va_arg(args, int);
	    format++; 
	    c = *format;
	}
	if (c == 'l') {			/* Ignored for compatibility. */
	    format++; 
	    c = *format;
	}
	if (c == 'h') {			/* Ignored for compatibility. */
	    format++; 
	    c = *format;
	}

	/*
	 * Take action based on the format type (which is now in c).
	 */

	field = buf;
	switch (c) {

	    case 'D':
	    case 'd':
	    case 'i':
		i = va_arg(args, int);
		if (i < 0) {
		    prefix = "-";
		    i = -i;
		    actualLength = 1;
		} else {
		    prefix = signChar;
		    actualLength = *prefix ? 1 : 0;
		}
		field = CvtUtoA((unsigned) i, 10, buf, &tmp, 0);
		if (tmp < precision) {
		    zeroPad = precision-tmp;
		}
		actualLength += tmp+zeroPad;
		break;
	    
	    case 'O':
	    case 'o':
		i = va_arg(args, int);
		field = CvtUtoA((unsigned) i, 8, buf, &tmp, 0);
		if (tmp < precision) {
		    zeroPad = precision-tmp;
		}
		if (altForm && (i != 0) && zeroPad==0) {
		    prefix = "0";
		    actualLength = 1;
		}
		actualLength += tmp+zeroPad;
		break;
	    
	    case 'X':
	    case 'x':
	    case 'p':
		i = va_arg(args, int);
		field = CvtUtoA((unsigned) i, 16, buf, &actualLength, c=='X');
		if (actualLength < precision) {
		    zeroPad = precision-actualLength;
		    actualLength += zeroPad;
		}
		if (altForm) {
		    char *p;
		    if (c == 'X') {
			if (i != 0) {
			    prefix = "0X";
			    actualLength += 2;
			}
			for (p = field; *p != 0; p++) {
			    if (*p >= 'a') {
				*p += 'A' - 'a';
			    }
			}
		    } else if (i != 0) {
			prefix = "0x";
			actualLength += 2;
		    }
		}
		break;
	    
	    case 'U':
	    case 'u':
		field = CvtUtoA(va_arg(args, unsigned), 10, buf,
			&actualLength, 0);
		if (actualLength < precision) {
		    zeroPad = precision-actualLength;
		    actualLength += zeroPad;
		}
		break;
	    
	    case 's':
		field = va_arg(args, char *);
		if (field == (char *) NULL) {
		    field = "(NULL)";
		} 
		actualLength = strlen(field);
		if ((precision >= 0) && (precision < actualLength)) {
		    actualLength = precision;
		}
		pad = ' ';
		break;

	    case 'c':
		buf[0] = va_arg(args, int);
		actualLength = 1;
		pad = ' ';
		break;

	    case 'F':
	    case 'f':
		if (precision < 0) {
		    precision = 6;
		} else if (precision > CVT_DBL_BUF_SIZE) {
		    precision = CVT_DBL_BUF_SIZE;
		}

		/*
		 * Just generate the digits and compute the total length
		 * here.  The rest of the work will be done when the
		 * characters are actually output, below.
		 */
		actualLength = CvtFtoA(va_arg(args, double), CVT_DBL_BUF_SIZE,
		    precision, &point, &sign, field, &fpError);

		if (sign) {
		    prefix = "-";
		    actualLength += 1;
		} else {
		    prefix = signChar;
		    actualLength += *prefix ? 1 : 0;
		}
		if (fpError) {
		    pad = ' ';
		    break;
		}
		if (point <= 0) {
		    actualLength += 1 - point;
		}
		if ((precision != 0) || (altForm)) {
		    actualLength += 1;
		}
		c = 'f';
		break;

	    case 'E':
	    case 'e':
		if (precision < 0) {
		    precision = 6;
		} else if (precision > CVT_DBL_BUF_SIZE-1) {
		    precision = CVT_DBL_BUF_SIZE-1;
		}
		actualLength = CvtFtoA(va_arg(args, double), precision+1, -1,
			&point, &sign, &buf[1], &fpError);
		if (fpError) {
		    pad = ' ';
		    field = &buf[1];
		    if (sign) {
			prefix = "-";
			actualLength += 1;
		    } else {
			prefix = signChar;
			actualLength += *prefix ? 1 : 0;
		    }
		    break;
		}
		eFromG:

		/*
		 * Insert a decimal point after the first digit of the number.
		 * If no digits after decimal point, then don't print decimal
		 * unless in altForm.
		 */

		buf[0] = buf[1];
		buf[1] = '.';
		if ((precision != 0) || (altForm)) {
		    field = buf + precision + 2;
		} else {
		    field = &buf[1];
		}

		/*
		 * Convert the exponent.
		 */
		
		*field = c;
	    	field++;
		point--;	/* One digit before decimal point. */
		if (point < 0) {
		    *field = '-';
		    point = -point;
		} else {
		    *field = '+';
		}
		field++;
		if (point < 10) {
		    *field = '0';
		    field++;
		}
		strcpy(field, CvtUtoA((unsigned) point, 10, expBuf, &i, 0));
		actualLength = (field - buf) + i;
		field = buf;
		if (sign) {
		    prefix = "-";
		    actualLength += 1;
		} else {
		    prefix = signChar;
		    actualLength += *prefix ? 1 : 0;
		}
		break;

	    case 'G':
	    case 'g': {
		int eLength, fLength;

		if (precision < 0) {
		    precision = 6;
		} else if (precision > CVT_DBL_BUF_SIZE-1) {
		    precision = CVT_DBL_BUF_SIZE-1;
		} else if (precision == 0) {
		    precision = 1;
		}

		actualLength = CvtFtoA(va_arg(args, double), precision,
			-1, &point, &sign, &buf[1], &fpError);

		if (fpError) {
		    pad = ' ';
		    field = &buf[1];
		    if (sign) {
			prefix = "-";
			actualLength += 1;
		    } else {
			prefix = signChar;
			actualLength += *prefix ? 1 : 0;
		    }
		    break;
		}
		if (!altForm) {
		    for ( ; actualLength > 1; actualLength--) {
			if (buf[actualLength] != '0') {
			    break;
			}
		    }
		}
		if ((actualLength > 1) || altForm) {
		    eLength = actualLength + 5;
		} else {
		    eLength = actualLength + 4;
		}
		if (point <= 0) {
		    fLength = actualLength + 2 - point;
		} else {
		    fLength = actualLength;
		    if (point < actualLength) {
			fLength += 1;
		    } else if (altForm) {
			fLength = point + 1;
		    } else {
			fLength = point;
		    }
		}

		/*
		 * Use "e" format if it results in fewer digits than "f"
		 * format, or if it would result in non-significant zeroes
		 * being printed.  Remember that precision means something
		 * different in "e" and "f" (digits after decimal) than it
		 * does in "g" (significant digits).
		 */

		if ((eLength < fLength) || (point > precision)) {
		    c += 'E' - 'G';
		    precision = actualLength-1;
		    goto eFromG;
		}
		c = 'f';
		field = &buf[1];
		actualLength = fLength;
		if (sign) {
		    prefix = "-";
		    actualLength += 1;
		} else {
		    prefix = signChar;
		    actualLength += *prefix ? 1 : 0;
		}
		break;
	    }

	    case '%':
		AddChar('%', stream);
		charsPrinted += 1;
		goto endOfField;

	    case 0:
		return charsPrinted;

	    default:
		AddChar(c, stream);
		charsPrinted += 1;
		goto endOfField;
	}

	/*
	 * Things get tricky if we want to pad with 0's and left justify.
	 */
	if (leftAdjust && pad == '0') {
	    if (c=='f' || c=='F' || c=='e' || c=='E' || c=='g' || c=='G') {
		pad = ' ';
	    } else {
		leftAdjust = FALSE;
	    }
	}

	/* Handle pad characters on the left.  If the pad is '0', then
	 * padding goes after the prefix.  Otherwise, padding goes before
	 * the prefix.
	 */

	if (!leftAdjust) {
	    if (pad == '0') {
		for ( ; *prefix != 0; prefix++) {
		    AddChar(*prefix, stream);
		    charsPrinted += 1;
		    actualLength--;
		    minWidth--;
		}
	    }
	    while (minWidth > actualLength) {
		AddChar(pad, stream);
		charsPrinted += 1;
		minWidth --;
	    }
	}

	/*
	 * Output anything left in the prefix.
	 */

	minWidth -= actualLength;
	for ( ; *prefix != 0; prefix++) {
	    AddChar(*prefix, stream);
	    charsPrinted += 1;
	    actualLength--;
	}

	/*
	 * Pad the digits to the right length with 0's.
	 */
	for (; zeroPad>0; zeroPad--) {
	    AddChar('0', stream);
	    charsPrinted += 1;
	    actualLength--;
	}

	/*
	 * "F" and "f" formats are handled specially here:  output
	 * everything up to and including the decimal point.
	 */

	if (c == 'f' && !fpError) {
	    if (point <= 0) {
		if (actualLength > 0) {
		    AddChar('0', stream);
		    charsPrinted += 1;
		    point++;
		    actualLength--;
		}
		if (actualLength > 0) {
		    charsPrinted += 1;
		    AddChar('.', stream);
		    actualLength--;
		}
		while ((point <= 0) && (actualLength > 0)) {
		    AddChar('0', stream);
		    charsPrinted += 1;
		    point++;
		    actualLength--;
		}
	    } else {
		while ((point > 0) && (actualLength > 0)) {
		    AddChar(*field, stream);
		    charsPrinted += 1;
		    field++;
		    point--;
		    actualLength--;
		}
		if (actualLength > 0) {
		    AddChar('.', stream);
		    charsPrinted += 1;
		    actualLength--;
		}
	    }
	}

	/*
	 * Output the contents of the field (for "f" format, this is
	 * just the stuff after the decimal point).
	 */

	charsPrinted += actualLength;
	for ( ; actualLength > 0; actualLength--, field++) {
	    AddChar(*field, stream);
        }

	/*
	 * Pad the right of the field, if necessary.
	 */

	while (minWidth > 0) {
	    AddChar(' ', stream);
	    charsPrinted += 1;
	    minWidth --;
	}

	endOfField: continue;
    }
    return charsPrinted;
}

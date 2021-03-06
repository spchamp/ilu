/* ilujava_conf.h.  Generated automatically by configure.  */
/*
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

$Id: ilujava_conf.h.in,v 1.2 1999/08/03 01:59:24 janssen Exp $
*/
/* To be processed by Autoconf */
/* Chris Jacobi, November 18, 1998 5:08 pm PST */

#ifndef	_ILUJAVA_CONF_H_
#define	_ILUJAVA_CONF_H_



/* 
 * +undef|define+ will be defined to major version of java (numeric)
 * the "1" of 1.2.3
 */
#define ILUJAVA_H_MAJORVERSION 1


/*
 * +undef|define+ will be defined to major version of java (numeric)
 * the "2" of 1.2.3
 */
#define ILUJAVA_H_MINORVERSION 2


/*
 * +undef|define+ will be defined if the version is JDK-1.2-beta2
 * not 1, 3 or 4 etc
 */
/* #undef ILUJAVA_H_JDK12BETA2 */


/*
 * +undef|define+ will be defined if the version is JDK-1.2-beta3
 * not 1, 2 or 4 etc
 */
/* #undef ILUJAVA_H_JDK12BETA3 */


/*
 * +undef|define+ will be defined if the version is JDK-1.2-beta4
 * not 1, 2, 3 or rc etc
 */
/* #undef ILUJAVA_H_JDK12BETA4 */


/*
 * +undef|define+ will be defined if the version is JDK-1.2fcs-rcx, or JDK-1.2 
 * not beta
 */
#define ILUJAVA_H_JDK12POSTBETA 1


#endif	/* _ILUJAVA_CONF_H_ */

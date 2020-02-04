/***************************************************************************//**
 * @file v.h
 * @brief	include files, debug print, etc...
 * @author hiroshi murakami
 * @date	20170519
 *
 * This software is released under the MIT License, see LICENSE.md
 ******************************************************************************/

#ifndef _V_H    /* Guard against multiple inclusion */
#define _V_H

#define _V_DEBUG		// when it's debugging, define _V_DEBUG_USB1. 

#ifdef _V_DEBUG
	#define	DEBUG_PUTS(str1)	xputs(str1)
	#define	DEBUG_PRINTF(fmt1, ...)	xprintf(fmt1, __VA_ARGS__)
#else
	#define	DEBUG_PUTS(str1)
	#define	DEBUG_PRINTF(fmt1, ...)
#endif


//#define vFLAG_ON  	1
//#define vFLAG_OFF  	0

#endif


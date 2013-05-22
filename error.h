/***************************************************************
 *
 * error.h
 * This module handles warnings messages, error messages and
 * fatal errors.
 *
 ***************************************************************
 *
 * This software was written in 2013 by the following author(s):
 * Brendan A R Sechter <bsechter@sennue.com>
 *
 * To the extent possible under law, the author(s) have
 * dedicated all copyright and related and neighboring rights
 * to this software to the public domain worldwide. This
 * software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain
 * Dedication along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * Please release derivative works under the terms of the CC0
 * Public Domain Dedication.
 *
 ***************************************************************/

#ifndef ERROR_H
#define ERROR_H

// library headers
#include <stdlib.h>

// constants
#define ERROR_MESSAGE_FORMAT      "%s : %s\n"
#define ERROR_MESSAGE_FATAL_ERROR "FATAL ERROR"
#define ERROR_MESSAGE_ERROR       "ERROR"
#define ERROR_MESSAGE_WARNING     "WARNING"

// macros
#define ERROR_STRINGIFY(pM) #pM
#define ERROR_TOSTRING(pM)  ERROR_STRINGIFY(pM)
#ifdef _DEBUG
  #define ERROR_MESSAGE(pM)   (pM " : " __FILE__ " (" ERROR_TOSTRING(__LINE__) ")")
#else  // _DEBUG undefined
  #define ERROR_MESSAGE(pM)   (pM)
#endif // _DEBUG

// macro functions
#define FATAL_ERROR(pMessage) fatalError(ERROR_MESSAGE(pMessage), EXIT_FAILURE)
#define ERROR(pMessage)       error     (ERROR_MESSAGE(pMessage))
#define WARNING(pMessage)     warning   (ERROR_MESSAGE(pMessage))

// function prototypes
void fatalError (const char *pMessage, int pExitCode);
void error      (const char *pMessage);
void warning    (const char *pMessage);

#endif // ERROR_H


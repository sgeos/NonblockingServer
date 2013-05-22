/***************************************************************
 *
 * error.c
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

// library headers
#include <stdlib.h>
#include <stdio.h>

// module headers
#include "error.h"

// display message in standard format
void errorMessage(const char *pTag, const char *pMessage)
{
  fprintf(stderr, ERROR_MESSAGE_FORMAT, pTag, pMessage);
  fflush (stderr); 
}

// display fatal error message and exit
void fatalError(const char *pMessage, int pExitCode)
{
  errorMessage(ERROR_MESSAGE_FATAL_ERROR, pMessage);
  perror      (ERROR_MESSAGE_FATAL_ERROR " ");
  exit        (pExitCode);
}

// display error message
void error(const char *pMessage)
{
  errorMessage(ERROR_MESSAGE_ERROR, pMessage);
}

// display warning message
void warning(const char *pMessage)
{
  errorMessage(ERROR_MESSAGE_WARNING, pMessage);
}


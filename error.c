/***************************************************************
 *
 * error.c
 * ...
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

#include <stdlib.h>
#include <stdio.h>

#include "error.h"

void errorMessage(const char *pTag, const char *pMessage)
{
  fprintf(stderr, ERROR_MESSAGE_FORMAT, pTag, pMessage);
  fflush (stderr); 
}

void fatalError(const char *pMessage, int pExitCode)
{
  errorMessage(ERROR_MESSAGE_FATAL_ERROR, pMessage);
  perror      (ERROR_MESSAGE_FATAL_ERROR " ");
  exit        (pExitCode);
}

void error(const char *pMessage)
{
  errorMessage(ERROR_MESSAGE_ERROR, pMessage);
}

void warning(const char *pMessage)
{
  errorMessage(ERROR_MESSAGE_WARNING, pMessage);
}

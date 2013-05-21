/***************************************************************
 *
 * args.h
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

#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

// types
typedef struct args_param_t args_param_t;
typedef int (*args_func_t)(int argc, char *argv[], int argn, args_param_t *args_param, void *data);
struct args_param_t
{
  const char  *flag;
  void        *data;
  args_func_t func;
};

// constants
#define ARGS_DEFAULT_TOKEN '-'
#define ARGS_FINAL_FLAG    NULL
#define ARGS_FINAL_DATA    NULL
#define ARGS_FINAL_FUNC    NULL

// final argument in list
#define ARGS_DONE       { ARGS_FINAL_FLAG, ARGS_FINAL_DATA, ARGS_FINAL_FUNC }

// process command line arguments
void argsProcess (int argc, char *argv[], args_param_t *pParams);

// scan basic types of command line arguments
int  argsInteger  ( int argc, char *argv[], int argn, struct args_param_t *args_param, void *data );
int  argsFloat    ( int argc, char *argv[], int argn, struct args_param_t *args_param, void *data );
int  argsString   ( int argc, char *argv[], int argn, struct args_param_t *args_param, void *data );
char argsGetToken ( void );
void argsSetToken ( char pToken );

#endif // ARGS_H


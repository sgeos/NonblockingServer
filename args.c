/***************************************************************
 *
 * args.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// module headers
#include "args.h"

// module variables
char mToken = ARGS_DEFAULT_TOKEN;

char argsGetToken(void)
{
  return mToken;
}

void argsSetToken(char pToken)
{
  mToken = pToken;
}

int argsNextData(int argc, char *argv[], int argn, struct args_param_t *args_param, const char **data)
{
  char         token = argsGetToken();
  const char * arg   = argv[argn];
  const char * flag  = args_param->flag;
  int          size  = strlen(flag);

  // no more command line arguments
  if (argc <= argn)
  {
    *data = NULL;
    return 0;
  }

  // data parameter
  if (token != argv[argn][0])
  {
    *data = argv[argn];
    return 1;
  }

  // contains flag
  else if (0 == strncmp(flag, arg, size))
  {
    // data after flag
    if ('\0' != argv[argn][size])
    {
      *data = &argv[argn][size];
      return 1;
    }
    // data in next parameter?
    else
    {
      return argsNextData(argc, argv, argn + 1, args_param, data) + 1;
    }
  }
  // else, it was an unrelated flag
  *data = NULL;
  return 0;
}

int argsInteger(int argc, char *argv[], int argn, struct args_param_t *args_param, void *data)
{
  int *        value_ptr = (int *)data;
  const char * argText;
  int          result = argsNextData(argc, argv, argn, args_param, &argText);
  if (argText)
  {
    *value_ptr = atoi(argText);
  }
  return result;
}

int argsFloat(int argc, char *argv[], int argn, struct args_param_t *args_param, void *data)
{
  float *      value_ptr = (float *)data;
  const char * argText;
  int          result = argsNextData(argc, argv, argn, args_param, &argText);
  if (argText)
  {
    *value_ptr = atof(argText);
  }
  return result;
}

int argsString(int argc, char *argv[], int argn, struct args_param_t *args_param, void *data)
{
  const char ** value_ptr = (const char **)data;
  const char *  argText;
  int           result = argsNextData(argc, argv, argn, args_param, &argText);
  if (argText)
  {
    *value_ptr = argText;
  }
  return result;
}

int argsParamEntryProcess(int argc, char *argv[], int argn, args_param_t *args_param)
{
  const char * arg    = argv[argn];
  const char * flag   = args_param->flag;
  int          size   = strlen(flag);
  int          result = 0;

  // call func if arg and flag are the same
  if (0 == strncmp(flag, arg, size))
  {
    args_func_t func = args_param->func;
    void *      data = args_param->data;

    result = func(argc, argv, argn, args_param, data);
    if (result < 1)
    {
      result = 1;
    }
  }

  // return number of args consumed
  return result;
}

int argsParamsProcess(int argc, char *argv[], int argn, args_param_t *pParams)
{
  int result = 0;
  int i;

  for (i = 0; ARGS_FINAL_FLAG != pParams[i].flag; i++)
  {
    result = argsParamEntryProcess(argc, argv, argn, &pParams[i]);
    if (result)
    {
      break;
    }
  }

  // return number of args consumed
  return result;
}

void argsProcess(int argc, char *argv[], args_param_t *pParams)
{
  int inc = 0;
  int i;

  for (i = 1; i < argc; i += inc)
  {
    inc = argsParamsProcess(argc, argv, i, pParams);
    if (inc < 1)
    {
      inc = 1;
    }
  }
}

// test program
#ifdef ARGS_MAIN

// program usage
int usage(int argc, char *argv[], int argn, args_param_t *argsparam, void *data)
{
  printf("Usage: %s [params]\n", argv[0]);
  printf("    -c     <transaction cost>\n");
  printf("    --cost <transaction cost>\n");
  printf("        Set transaction cost.\n");
  printf("    -p     <port number>\n"     );
  printf("    --port <port number>\n"     );
  printf("        Set port number.\n"     );
  printf("    -h     <host name>\n"       );
  printf("    --host <host name>\n"       );
  printf("        Set host.\n"            );
  printf("    -?\n"                       );
  printf("    --help\n"                   );
  printf("        Print this usage.\n"    );
  exit(EXIT_SUCCESS);
  return 1;
}

// main program
int main(int argc, char *argv[])
{
  int    port = 12;
  char * host = "localhost";
  float  cost = 3.14;

  args_param_t args_param_list[] =
  {
    {"-c",     &cost, argsFloat  },
    {"--cost", &cost, argsFloat  },
    {"-h",     &host, argsString },
    {"--host", &host, argsString },
    {"-p",     &port, argsInteger},
    {"--port", &port, argsInteger},
    { "-?",     NULL, usage      },
    { "--help", NULL, usage      },
    ARGS_DONE
  };
  argsProcess(argc, argv, args_param_list);
  printf("Port: %d\n", port);
  printf("Host: %s\n", host);
  printf("Cost: %f\n", cost);
  return 0;
}

#endif // ARGS_MAIN


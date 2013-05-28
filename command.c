/***************************************************************
 *
 * command.c
 * This module handles commands used for client / server
 * communication.
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// module headers
#include "command.h"

// this function scans data into variables using a format string
// call it inside of a callback function to populate variables
int commandScan(const char *pBuffer, char *pFormat, ...)
{
  // local variables
  int     result;
  va_list arguments;

  va_start(arguments, pFormat);
  result = vsscanf(pBuffer, pFormat, arguments);
  va_end(arguments);
  return result;
}

// this function tests the buffer to see if it matches the specified command
// the data after the match is returned on success, NULL is returned on failure
const char *commandMatch(const char *pBuffer, const char *pCommand)
{
  int size = strlen(pCommand);

  // if this is the correct command
  if (0 == strncmp(pCommand, pBuffer, size))
  {
    // return the data after the command
    return &pBuffer[size];
  }
  // else
  return NULL;
}

// tests a single command for a match
// the callback function is called if there is a match
int commandEntryProcess(const char *pBuffer, command_param_t *pCommandParameter)
{
  if (commandMatch(pBuffer, pCommandParameter->command))
  {
    return pCommandParameter->callback(pBuffer, pCommandParameter, pCommandParameter->data);
  }
  // else
  return 0;
}

// tests the buffer for a match in a list of commands
// the callback function is called if there is a match 
int commandProcess(const char *pBuffer, command_param_t *pCommandList)
{
  // local variables
  int result = 0;
  int i;

  // try every command
  for (i = 0; COMMAND_FINAL_COMMAND != pCommandList[i].command; i++)
  {
    result = commandEntryProcess(pBuffer, &pCommandList[i]);
    if (result)
    {
      break;
    }
  }
  return result;
}

// example program
#ifdef COMMAND_MAIN

// constants
#define COMMAND_BUFFER_SIZE 1500

// command to exit program
int commandScanDone(const char *pBuffer, command_param_t *pCommandParameter, void *pData)
{
  void **      dataParameter = (void **)pData;
  char *       message       = dataParameter[0];
  int *        exitCode      = dataParameter[1];
  int *        done          = dataParameter[2];
  const char * scanBuffer    = commandMatch(pBuffer, pCommandParameter->command);
  int          result        = commandScan(scanBuffer, "%d/%s", exitCode, message);
  *done = 1;
  //printf("DONE\n");
  //printf("Message:   %s\n",  message);
  //printf("Exit Code: %d\n", *exitCode);
  //exit(*exitCode);
  return result;
}

// command to display message
int commandScanMessage(const char *pBuffer, command_param_t *pCommandParameter, void *pData)
{
  const char * message = commandMatch(pBuffer, pCommandParameter->command);

  printf("MESSAGE\n");
  printf("Message:   %s\n", message);
  return 1;
}

// main program
int main(void)
{
  char   buffer [COMMAND_BUFFER_SIZE] = "";
  char   message[COMMAND_BUFFER_SIZE] = "";
  int    exitCode                     = 0;
  int    done                         = 0;
  void * dataDone[]                   = {message, &exitCode, &done};
  command_param_t commandList[] =
  {
    {"/done/",    commandScanDone,    (void *)dataDone},
    {"/exit/",    commandScanDone,    (void *)dataDone},
    {"/quit/",    commandScanDone,    (void *)dataDone},
    {"/message/", commandScanMessage, NULL            },
    COMMAND_DONE
  };

  // command loop
  while (!done)
  {
    printf("> ");
    fflush(stdout);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer)-1] = '\0';
    commandProcess(buffer, commandList);
  }

  // all done
  printf("Exit Code : %d\n%s\n", exitCode, message);
  return exitCode;
}

#endif // COMMAND_MAIN


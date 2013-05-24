/***************************************************************
 *
 * server.c
 * The main module for a simple chat server written in C.
 * It uses non-blocking sockets and non-blocking terminal input.
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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// module headers
#include "args.h"
#include "database.h"
#include "error.h"
#include "message.h"
#include "network.h"
#include "server.h"
#include "terminalInput.h"

// log message
void logMessage(int pSocketId, server_state_t *pState)
{
  char * message  = pState->writeBuffer;
  int    isSystem = (pSocketId == pState->newConnectionSocket);
  char username[] = "User XXXXXXXXXX";

  // set username
  if (isSystem)
  {
    snprintf(username, sizeof(username), "System");
  }
  else // user
  {
    snprintf(username, sizeof(username), "User %d", pSocketId);
  }

  // write to database
  database_addMessage(username, message);
}

// connect client to server
void connectClient(int pSocketId, server_state_t *pState)
{
  // connect
  int newSocketId  = getConnection(pSocketId);
  FD_SET(newSocketId, &(pState->readSocketSet));
  FD_SET(newSocketId, &(pState->writeSocketSet));

  // need to print after connecting
  printf("Connected Client : id %d, host %s, port %hu\n", newSocketId, networkSocketHost(newSocketId), networkSocketPort(newSocketId));
}

// disconnect client from server
void disconnectClient(int pSocketId, server_state_t *pState)
{
  // need to print before disconnecting
  printf("Disconnected Client : id %d, host %s, port %hu\n", pSocketId, networkSocketHost(pSocketId), networkSocketPort(pSocketId));

  // disconnect
  close (pSocketId);
  FD_CLR(pSocketId, &(pState->readSocketSet));
  FD_CLR(pSocketId, &(pState->writeSocketSet));
}

// broadcast message
void forwardMessage(int pSocketId, server_state_t *pState)
{
  fd_set broadcastSocketSet = pState->writeSocketSet;
  char * message            = pState->writeBuffer;

  // do not return message to sender
  FD_CLR(pSocketId, &broadcastSocketSet);

  // broadcast message
  broadcastMessage(&broadcastSocketSet, FD_SETSIZE, message);

  // display output
  printf("Message : %s\n", message);
  printf("From : %d\n", pSocketId);
  printf("To :");
  int i;
  for (i = 0; i < FD_SETSIZE; i++)
  {
    if (FD_ISSET(i, &broadcastSocketSet))
    {
      printf(" %d", i);
    }
  }
  printf("\n");

  // log message
  logMessage(pSocketId, pState);
}

// read message from client
int readClientMessage(int pSocketId, server_state_t *pState)
{
  char * message = pState->writeBuffer;
  int    result;

  if (receiveMessageReady(pSocketId))
  {
    result = receiveMessage(pSocketId, message, NETWORK_COMMUNICATION_BUFFER_SIZE);
    if (result)
    {
      forwardMessage(pSocketId, pState);
    }
  }
  else
  {
    // no data, no problem
    result = 1;
  }
  return result;
}

// process input from sockets
void processSocketInput(server_state_t *pState)
{
  fd_set loopSocketSet = pState->readSocketSet;
  int    printCounter  = 0;

  // service all sockets
  while (networkSocketReady(&loopSocketSet))
  {
    for (int i = 0; i < FD_SETSIZE; i++)
    {
      if (FD_ISSET(i, &loopSocketSet))
      {
        if (0 == printCounter && !terminalInputEmpty())
        {
          printf("\n");
        }
        if (i == pState->newConnectionSocket)
        {
          connectClient(i, pState);
        }
        else
        {
          if (!readClientMessage(i, pState))
          {
            disconnectClient(i, pState);
            FD_CLR(i, &loopSocketSet);
          }
        }
        printCounter++;
      }
    }
  }
  if (0 < printCounter)
  {
    terminalInputPromptDisplayUnlessEmpty();
  }
}

// act on user input
void executeCommand(server_state_t *pState)
{
  const char * command        = pState->readBuffer;
  int          systemSocketId = pState->newConnectionSocket;

  if (0 == strcmp(command, MESSAGE_EXIT))
  {
    pState->done = 1;
  }
  else if (0 == strcmp(command, MESSAGE_NULL))
  {
    printf("%s %s\n", terminalInputGetPrompt(), MESSAGE_DEFAULT);
    forwardMessage(systemSocketId, pState);
  }
  else
  {
    strncpy(pState->writeBuffer, pState->readBuffer, NETWORK_COMMUNICATION_BUFFER_SIZE);
    forwardMessage(systemSocketId, pState);
  }
  terminalInputReset();
}

// process user input
void processInput(server_state_t *pState)
{
  int isBufferEmpty = terminalInputEmpty();

  while (terminalInputReady())
  {
    int clearLine = 0;
    int c = fgetc(stdin);
    switch (c)
    {
      case '\n':
        executeCommand(pState);
        break;
      case '\b':
      case 127:
      case 224:
        printf("\n");
        if (clearLine)
        {
          terminalInputReset();
        }
        else
        {
          terminalInputBackspace();
          terminalInputPromptDisplayUnlessEmpty();
        }
        break;
      default:
        terminalInputBufferCharacter(c);
        if (isBufferEmpty)
        {
          printf("\n");
          terminalInputPromptDisplay();
        }
        break;
    }
  }
}

// initialize server
void init(server_param_t *pParameters, server_state_t *pState)
{
  // program started
  pState->done = 0;

  // create socket and connect to server
  pState->newConnectionSocket = initServer(pParameters->port);

  // initialize socket sets
  FD_ZERO(&(pState->readSocketSet));
  FD_ZERO(&(pState->writeSocketSet));
  FD_SET (pState->newConnectionSocket, &(pState->readSocketSet));

  // initialize buffers
  bzero(pState->readBuffer,  NETWORK_COMMUNICATION_BUFFER_SIZE);
  bzero(pState->writeBuffer, NETWORK_COMMUNICATION_BUFFER_SIZE);

  // initialize external modules
  terminalInputInit(TERMINAL_INPUT_DEFAULT_PROMPT, pState->readBuffer,  NETWORK_COMMUNICATION_BUFFER_SIZE);
  terminalInputPromptDisplayUnlessEmpty();
  database_init();

  // print startup messages
  printf("Server started.\n");
  printf("Port: %d\n", pParameters->port);
}

// cleanup before exiting
void cleanup(server_state_t *pState)
{
  // cleanup external modules
  terminalInputCleanUp();
  database_close();

  // program ended
  pState->done = pState->done ? pState->done : EXIT_SUCCESS;

  // close socket
  close(pState->newConnectionSocket);

  // print message
  printf("Server stopped.\n");
}

// service loop
void service(server_state_t *pState)
{
  // service loop
  while (!pState->done)
  {
    processSocketInput(pState);
    processInput(pState);
  }
  pState->done = EXIT_SUCCESS;
}

// program usage
int usage(int argc, char *argv[], int argn, args_param_t *args_param, void *data)
{
  // print usage
  printf ( "Usage: %s [params]\n", argv[0] );
  printf ( "    -p     <port number>\n"    );
  printf ( "    --port <port number>\n"    );
  printf ( "        Set port number.\n"    );
  printf ( "    -?\n"                      );
  printf ( "    --help\n"                  );
  printf ( "        Print this usage.\n"   );

  // exit program
  exit(EXIT_SUCCESS);
  return 1;
}

// main program     
int main(int argc, char *argv[])
{
  server_state_t programState;
  server_param_t parameters =
  {
    NETWORK_DEFAULT_PORT
  };
  args_param_t args_param_list[] =
  {
    { "-p",     &parameters.port, argsInteger },
    { "--port", &parameters.port, argsInteger },
    { "-?",     &programState,    usage       },
    { "--help", &programState,    usage       },
    ARGS_DONE
  };

  // process command line arguments
  argsProcess(argc, argv, args_param_list);

  // initialize program state
  init(&parameters, &programState);

  // run service loop
  service(&programState);

  // cleanup
  cleanup(&programState);

  // exit
  return programState.done;
}


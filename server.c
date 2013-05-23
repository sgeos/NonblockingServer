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
#include "error.h"
#include "message.h"
#include "network.h"
#include "server.h"

void printClientInfo(int pSocketId)
{
  struct sockaddr_in clientAddress;
  socklen_t          size = sizeof(struct sockaddr_in);
  getsockname(pSocketId, (struct sockaddr *)&clientAddress, &size);
  printf("id %d, host %s, port %hu", pSocketId, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
}

void connectClient(int pSocketId, fd_set *pReadSet, fd_set *pWriteSet)
{
  // connect
  int newSocketId  = getConnection(pSocketId);
  FD_SET(newSocketId, pReadSet);
  FD_SET(newSocketId, pWriteSet);

  // need to print after disconnecting
  printf("Connected Client : ");
  printClientInfo(newSocketId);
  printf("\n");
}

void disconnectClient(int pSocketId, fd_set *pReadSet, fd_set *pWriteSet)
{
  // need to print before disconnecting
  printf("Disconnected Client : ");
  printClientInfo(pSocketId);
  printf("\n");

  // disconnect
  close (pSocketId);
  FD_CLR(pSocketId, pReadSet);
  FD_CLR(pSocketId, pWriteSet);
}

int isSocketReady(fd_set *pReadSet)
{
  struct timeval timeout;
  int            result;

  // initialize timeout
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  // check socket
  result = select(FD_SETSIZE, pReadSet, NULL, NULL, &timeout);
  if (result < 0)
  {
    FATAL_ERROR("Could not check sockets for input.");
  }
  return result;
}

void broadcastMessage(int pSocketId, fd_set *pWriteSet, const char *pMessage)
{
  printf("Message : %s\n", pMessage);
  printf("From : %d\n", pSocketId);
  printf("To :");
  int i;
  for (i = 0; i < FD_SETSIZE; i++)
  {
    if (FD_ISSET(i, pWriteSet))
    {
      if (i == pSocketId)
      {
        // do nothing
      }
      else
      {
        printf(" %d", i);
        sendMessage(i, pMessage);
      }
    }
  }
  printf("\n");
}

int readClientMessage(int pSocketId, fd_set *pWriteSet)
{
  char messageBuffer[NETWORK_COMMUNICATION_BUFFER_SIZE];
  int result = 1;

  if (receiveMessageReady(pSocketId))
  {
    result = receiveMessage(pSocketId, messageBuffer, sizeof(messageBuffer));
    if (result)
    {
      broadcastMessage(pSocketId, pWriteSet, messageBuffer);
    }
  }
  return result;
}

int processSocketInput(int pNewConnectionSocket, fd_set *pReadSet, fd_set *pWriteSet)
{
  int done = 0;

  // service all sockets
  fd_set loopSet = *pReadSet;
  while (isSocketReady(&loopSet))
  {
    for (int i = 0; i < FD_SETSIZE; i++)
    {
      if (FD_ISSET(i, &loopSet))
      {
        if (i == pNewConnectionSocket)
        {
          connectClient(i, pReadSet, pWriteSet);
        }
        else
        {
          if (!readClientMessage(i, pWriteSet))
          {
            disconnectClient(i, pReadSet, pWriteSet);
            FD_CLR(i, &loopSet);
          }
        }
      }
    }
  }
  return done;
}

// service loop
int service(int pPort)
{
  int       newConnectionSocket;
  fd_set    readSet;
  fd_set    writeSet;
  int       done;

  // create socket
  newConnectionSocket = initServer(pPort);

  // initialize socket sets
  FD_ZERO(&readSet);
  FD_ZERO(&writeSet);
  FD_SET (newConnectionSocket, &readSet);

  // service loop
  done = 0;
  while (!done)
  {
    done += processSocketInput(newConnectionSocket, &readSet, &writeSet);
  }

  return EXIT_SUCCESS;
}

// program usage
int usage(int argc, char *argv[], int argn, args_param_t *args_param, void *data)
{
  printf ( "Usage: %s [params]\n", argv[0] );
  printf ( "    -p     <port number>\n"    );
  printf ( "    --port <port number>\n"    );
  printf ( "        Set port number.\n"    );
  printf ( "    -?\n"                      );
  printf ( "    --help\n"                  );
  printf ( "        Print this usage.\n"   );
  exit(EXIT_SUCCESS);
  return 1;
}

// main program     
int main(int argc, char *argv[])
{
  server_param_t parameters =
  {
    NETWORK_DEFAULT_PORT
  };
  args_param_t args_param_list[] =
  {
    { "-p",     &parameters.port, argsInteger },
    { "--port", &parameters.port, argsInteger },
    { "-?",     NULL, usage                   },
    { "--help", NULL, usage                   },
    ARGS_DONE
  };
  int result;

  // process command line arguments
  argsProcess ( argc, argv, args_param_list );
  printf ( "Port: %d\n", parameters.port );

  // service loop
  result = service(parameters.port);

  // done
  return result;
}


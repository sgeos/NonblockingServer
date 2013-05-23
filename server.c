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

void connectClient(int pSocketId, fd_set *pReadSet, fd_set *pWriteSet)
{
  struct sockaddr_in clientName;
  socklen_t          size         = sizeof(clientName);
  int                newSocketId  = accept(pSocketId, (struct sockaddr *)&clientName, &size);
  if (newSocketId < 0)
  {
    FATAL_ERROR("Could not accept new client connection.");
  }
  printf("Connected Client : id %d, host %s, port %hu\n", newSocketId, inet_ntoa(clientName.sin_addr), ntohs(clientName.sin_port));
  FD_SET(newSocketId, pReadSet);
  FD_SET(newSocketId, pWriteSet);
}

void disconnectClient(int pSocketId, fd_set *pReadSet, fd_set *pWriteSet)
{
  printf("Disconnected Client : id %d\n", pSocketId);
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

int make_socket(uint16_t pPort)
{
  int                result;
  struct sockaddr_in name;

  // create socket
  result = socket(PF_INET, SOCK_STREAM, 0);
  if (result < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // name socket
  name.sin_family      = AF_INET;
  name.sin_port        = htons(pPort);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind socket
  if (bind(result, (struct sockaddr *)&name, sizeof(name)) < 0)
  {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  return result;
}

// service loop
int service()
{
  return 0;
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
  exit(0);
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
  int       newConnectionSocket;
  fd_set    readSet;
  fd_set    writeSet;
  int       done;

  // process command line arguments
  argsProcess ( argc, argv, args_param_list );
  printf ( "Port: %d\n", parameters.port );

  // create socket
  newConnectionSocket = make_socket(parameters.port);
  if (listen(newConnectionSocket, 1) < 0)
  {
    FATAL_ERROR("Could not listen for new client connections.");
  }

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
  return 0;
}


/***************************************************************
 *
 * network.c
 * This module contians network code for the client and server.
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
#include <netdb.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

// module headers
#include "error.h"
#include "network.h"

// open a socket
int openSocket(void)
{
  int result;

  result = socket(PF_INET, SOCK_STREAM, 0);
  if (result < 0)
  {
    FATAL_ERROR("Error opening socket!");
  }
  return result;
}

// bind a socket (server)
void bindSocket(int pSocketFile, int pPort)
{
  // local variables
  struct sockaddr_in address;
  int                size;

  // zero data
  size = sizeof(address);
  bzero ((char *)&address, size);

  // initialize data
  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port        = htons(pPort);

  // bind
  if (bind(pSocketFile, (struct sockaddr *)&address, size) < 0)
  {
    FATAL_ERROR("Error on binding!");
  }
}

// connect a socket (client)
void connectSocket(int pSocketFile, int pPort, const char *pHost)
{
  // local variables
  struct sockaddr_in address;
  struct hostent *   server;

  // get server info
  server = gethostbyname(pHost);
  if (NULL == server)
  {
    FATAL_ERROR("No such host!");
  }

  // initialize data
  bzero((char *)&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port   = htons(pPort);

  //address.sin_addr = *(struct in_addr *)server->h_addr;
  bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr, server->h_length);

  // connect
  if (connect(pSocketFile, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    FATAL_ERROR("Error connecting!");
  }

  // use non-blocking socket
  int on = 1;
  if (ioctl(pSocketFile, FIONBIO, (char *)&on) < 0)
  {
    FATAL_ERROR("Error using non-binding socket!");
  }
}

// server initialization
int initServer(int pPort)
{
  // local variables
  int socketFile;

  // open / bind / listen on socket
  socketFile = openSocket();
  bindSocket(socketFile, pPort);
  listen(socketFile, NETWORK_BACKLOG_QUEUE);

  return socketFile;
}

// client initialization
int initClient(int pPort, const char *pHost)
{
  // open socket
  int socketFile = openSocket();

  // connect to host
  connectSocket(socketFile, pPort, pHost);

  return socketFile;
}

// get a connection
int getConnection(int pSocketFile)
{
  struct sockaddr_in address;
  socklen_t          size;
  int                socketFile;

  size = sizeof(address);
  socketFile = accept(pSocketFile, (struct sockaddr *)&address, &size);
  if (socketFile < 0)
  {
    FATAL_ERROR("Error on accept!");
  }
  return socketFile;
}

// send message
void sendMessage(int pSocketFile, const char *pMessage)
{
  int size   = strlen(pMessage) + 1;
  int result = write(pSocketFile, pMessage, size);
  if (result < 0)
  {
    FATAL_ERROR("Error writing to socket!");
  }
}

// receive message
int receiveMessage(int pSocketFile, char *pBuffer, int pSize)
{
  bzero(pBuffer, pSize);
  int result = read(pSocketFile, pBuffer, pSize - 1);
  if (result < 0)
  {
    // Non-blocking, so a failed read is not an error.
    result = 0;
    // FATAL_ERROR("Error reading from socket!");
  }
  return result;
}


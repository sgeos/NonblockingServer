/***************************************************************
 *
 * server.c
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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "message.h"
#include "error.h"

void write_to_client(int pFileDes, const char *pMessage)
{
  int size = write(pFileDes, pMessage, strlen(pMessage) + 1);
  if (size < 0)
  {
    FATAL_ERROR("write");
  }
}

int read_from_client(int pFileDes, fd_set *pWriteFdSet)
{
  char messageBuffer[DEFAULT_MESSAGE_BUFFER_SIZE];
  int  size = read(pFileDes, messageBuffer, DEFAULT_MESSAGE_BUFFER_SIZE);
  int  result;

  if (size < 0)
  {
    FATAL_ERROR("read");
  }
  else if (size == 0)
  {
    // end of file
    result = -1;
  }
  else
  {
    // data read
    fprintf(stderr, "Message : %s\n", messageBuffer);
    result = 0;
  }

  // service all sockets
  printf("From : %d\n", pFileDes);
  printf("To :");

  int i;
  for (i = 0; i < FD_SETSIZE; i++)
  {
    if (FD_ISSET(i, pWriteFdSet))
    {
      if (i == pFileDes)
      {
        // do nothing
      }
      else
      {
        printf(" %d", i);
        write_to_client(i, messageBuffer);
      }
    }
  }
  printf("\n");

  return result;
}
     
int main(void)
{
  extern int make_socket(uint16_t port);
  int sock;
  fd_set active_fd_set, read_fd_set, write_fd_set;
  int i;
  struct sockaddr_in clientname;
  socklen_t size;
  int done = 0;

  // create socket
  sock = make_socket(DEFAULT_PORT);
  if (listen(sock, 1) < 0)
  {
    FATAL_ERROR("listen");
  }

  // initialize set of active sockets
  FD_ZERO(&active_fd_set);
  FD_SET(sock, &active_fd_set);

  // initialize set of write sockets
  FD_ZERO(&write_fd_set);

  while (!done)
  {
    // block until input arrives on a socket
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
    {
      FATAL_ERROR("select");
    }

    // service all sockets
    for (i = 0; i < FD_SETSIZE; i++)
    {
      if (FD_ISSET(i, &read_fd_set))
      {
        if (i == sock)
        {
          // connection request on original socket
          int new;
          size = sizeof(clientname);
          new  = accept(sock, (struct sockaddr *)&clientname, &size);
          if (new < 0)
          {
            FATAL_ERROR("accept");
          }
          fprintf(stderr, "Connection : host %s, port %hd\n", inet_ntoa (clientname.sin_addr), ntohs (clientname.sin_port));
          FD_SET(new, &active_fd_set);
          FD_SET(new, &write_fd_set);
        }
        else
        {
          // data arriving on an already connected socket
          if (read_from_client(i, &write_fd_set) < 0)
          {
            close(i);
            FD_CLR(i, &active_fd_set);
            FD_SET(i, &write_fd_set);
          }
        }
      }
    }
  }
  return 0;
}


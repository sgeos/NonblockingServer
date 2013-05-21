/***************************************************************
 *
 * MERGE ME
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
#include <sys/socket.h>
#include <netinet/in.h>

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


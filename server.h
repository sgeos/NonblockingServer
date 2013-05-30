/***************************************************************
 *
 * server.h
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

#ifndef SERVER_H
#define SERVER_H

// constants
#define SERVER_DEFAULT_ECHO 1

// struct containing command line parameters
typedef struct server_param_t
{
  int port;
  int echoMessage;
} server_param_t;

// struct containing program state
typedef struct server_state_t
{
  int    done;
  int    echoMessage;
  int    newConnectionSocket;
  fd_set readSocketSet;
  fd_set writeSocketSet;
  char   readBuffer [NETWORK_COMMUNICATION_BUFFER_SIZE];
  char   writeBuffer[NETWORK_COMMUNICATION_BUFFER_SIZE];
} server_state_t;

#endif  // SERVER_H


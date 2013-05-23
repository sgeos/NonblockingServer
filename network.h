/***************************************************************
 *
 * network.h
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

#ifndef NETWORK_H
#define NETWORK_H

// library headers
#include <netinet/in.h>

// constants
#define NETWORK_DEFAULT_HOST              "localhost"
#define NETWORK_DEFAULT_PORT              51717
#define NETWORK_BACKLOG_QUEUE             5
#define NETWORK_COMMUNICATION_BUFFER_SIZE 1024

// function prototypes
int  openSocket          (void);
void bindSocket          (int pSocketFile, int pPort);
int  initServer          (int pPort);
int  initClient          (int pPort, const char *pHost);
int  getConnection       (int pSocketFile);
void sendMessage         (int pSocketFile, const char *pMessage);
int  receiveMessage      (int pSocketFile, char *pBuffer, int pSize);
void broadcastMessage    (fd_set *pSocketSet, int pSocketSetSize, const char *pMessage);
int  receiveMessageReady (int pSocketFile);

#endif // NETWORK_H


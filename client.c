/***************************************************************
 *
 * client.c
 * The main module for a simple chat client written in C.
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
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// module headers
#include "args.h"
#include "client.h"
#include "error.h"
#include "message.h"
#include "terminalInput.h"

void writeToServer(int pFileDes, const char *pMessage)
{
  int size = write(pFileDes, pMessage, strlen(pMessage) + 1);
  if (size < 0)
  {
    FATAL_ERROR("write");
  }
}

int readFromServer(int pSocketFile, char *pBuffer, int pSize)
{
  bzero(pBuffer, pSize);
  int result = read(pSocketFile, pBuffer, pSize - 1);
  if (result < 0)
  {
    return 0;
    FATAL_ERROR("read");
  }
  return result;
}

int processMessage(int pSocketFile, char *pBuffer)
{
  int done = 0;
  if (0 == strcmp(pBuffer, MESSAGE_EXIT))
  {
    done = 1;
  }
  else if (0 == strcmp(pBuffer, MESSAGE_NULL))
  {
    printf("%s %s\n", terminalInputGetPrompt(), MESSAGE_DEFAULT);
    writeToServer(pSocketFile, MESSAGE_DEFAULT);
  }
  else
  {
    writeToServer(pSocketFile, pBuffer);
  }
  terminalInputPrompt();
  return done;
}

int processInput(int pSocketFile, char *pBuffer, int pSize)
{
  int done = 0;
  while (terminalInputReady())
  {
    int clearLine = 1;
    int c = fgetc(stdin);
    switch (c)
    {
      case '\n':
        done = processMessage(pSocketFile, pBuffer);
        break;
      case '\b':
      case 127:
      case 224:
        printf("\n");
        if (clearLine)
        {
          terminalInputPrompt();
        }
        else
        {
          terminalInputBackspace();
          terminalInputPromptDisplay();
        }
        break;
      default:
        terminalInputBufferCharacter(c);
        break;
    }
  }
  return done;
}

void processOutput(int pSocket, char *pBuffer, int pSize)
{
  int outputPrinted = 0;
  while (readFromServer(pSocket, pBuffer, pSize))
  {
    if (!outputPrinted)
    {
      outputPrinted = 1;
      printf("\n");
    }
    printf("Message : %s\n", pBuffer);
  }
  if (outputPrinted)
  {
    terminalInputPromptDisplay();
  }
}

void initSockAddr(struct sockaddr_in *pName, const char *pHostName, uint16_t pPort)
{
  struct hostent *hostinfo;

  pName->sin_family = AF_INET;
  pName->sin_port   = htons(pPort);
  hostinfo          = gethostbyname(pHostName);
  if (NULL == hostinfo)
  {
    FATAL_ERROR("unknown host");
  }
  pName->sin_addr = *(struct in_addr *)hostinfo->h_addr;
}

int init(client_param_t *pParameters)
{
  int result; // socket
  struct sockaddr_in servername;
  int status;

  // create socket
  result = socket(PF_INET, SOCK_STREAM, 0);
  if (socket < 0)
  {
    FATAL_ERROR("socket (client)");
  }

  // connect to server
  initSockAddr(&servername, pParameters->host, pParameters->port);
  if (0 > connect(result, (struct sockaddr *)&servername, sizeof(servername)))
  {
    FATAL_ERROR("connect (client)");
  }
  else
  {
    printf("Host: %s\n", pParameters->host);
    printf("Port: %d\n", pParameters->port);
  }
     
  // use non-blocking socket
  int on = 1;
  status = ioctl(result, FIONBIO, (char *)&on);

  return result;
}

void cleanup(int pSocketFile)
{
  terminalInputCleanUp();
  close(pSocketFile);
}

// service loop
int service(int pSocketFile)
{
  char inputBuffer [DEFAULT_MESSAGE_BUFFER_SIZE];
  char outputBuffer[DEFAULT_MESSAGE_BUFFER_SIZE];
  int  done;

  // initialize service loop
  done = 0;
  terminalInputInit(TERMINAL_INPUT_DEFAULT_PROMPT, inputBuffer, sizeof(inputBuffer));
  terminalInputPromptDisplay();

  // main loop
  while (!done)
  {
    processOutput(pSocketFile, outputBuffer, sizeof(outputBuffer));
    done = processInput (pSocketFile, inputBuffer,  sizeof(inputBuffer));
  }

  return EXIT_SUCCESS;
}

// program usage
int usage(int pArgC, char *pArgV[], int pArgN, args_param_t *pArgsParam, void *pData)
{
  printf("Usage: %s [params]\n", pArgV[0]);
  printf("    -p     <port number>\n"    );
  printf("    --port <port number>\n"    );
  printf("        Set port number.\n"    );
  printf("    -h     <port number>\n"    );
  printf("    --host <port number>\n"    );
  printf("        Set host.\n"           );
  printf("    -?\n"                      );
  printf("    --help\n"                  );
  printf("        Print this usage.\n"   );
  exit(EXIT_SUCCESS);
  return 1;
}

int main(int argc, char *argv[])
{
  client_param_t parameters =
  {
    DEFAULT_PORT,
    DEFAULT_SERVER_HOST
  };
  args_param_t args_param_list[] =
  {
    { "-p",     &parameters.port, argsInteger },
    { "--port", &parameters.port, argsInteger },
    { "-h",     &parameters.host, argsString  },
    { "--host", &parameters.host, argsString  },
    { "-?",     NULL,             usage       },
    { "--help", NULL,             usage       },
    ARGS_DONE
  };
  int socketFile;
  int result;

  // process command line arguments
  argsProcess(argc, argv, args_param_list);

  // initialize program
  socketFile = init(&parameters);

  // run service loop
  result = service(socketFile);

  // cleanup
  cleanup(socketFile);

  // exit
  return result;
}


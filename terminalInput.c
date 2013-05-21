/***************************************************************
 *
 * terminalInput.c
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
#include <stdlib.h>
#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>

#include "terminalInput.h"

char           mTerminalInputDefaultBuffer[TERMINAL_INPUT_BUFFER_SIZE];
char *         mTerminalInputBuffer;
int            mTerminalInputBufferSize;
const char *   mTerminalInputPrompt;
int            mTerminalInputCursor;
struct termios mTerminalInputOriginalSettings;

void terminalInputInit(const char *pPrompt, char *pBuffer, int pSize)
{
  struct termios newSettings;

  terminalInputSetPrompt(pPrompt);
  terminalInputSetBuffer(pBuffer, pSize);
  terminalInputReset();

  // Get the current terminal settings
  if (tcgetattr(0, &mTerminalInputOriginalSettings) < 0)
  {
    perror("tcgetattr()");
    exit(EXIT_FAILURE);
  }
  memcpy(&newSettings, &mTerminalInputOriginalSettings, sizeof(struct termios));

  // disable canonical mode processing in the line discipline driver
  newSettings.c_lflag &= ~ICANON;

  // apply our new settings
  if (tcsetattr(0, TCSANOW, &newSettings) < 0)
  {
    perror("tcsetattr ICANON");
    exit(EXIT_FAILURE);
  }
}

void terminalInputCleanUp(void)
{
  // restore the terminal settings to their prvious state
  if (tcsetattr(0, TCSANOW, &mTerminalInputOriginalSettings) < 0)
  {
    perror("tcsetattr ICANON");
    exit(EXIT_FAILURE);
  }
}

void terminalInputReset(void)
{
  bzero(mTerminalInputBuffer, mTerminalInputBufferSize);
  mTerminalInputCursor = 0;
}

void terminalInputSetPrompt(const char *pPrompt)
{
  mTerminalInputPrompt = pPrompt ? pPrompt : TERMINAL_INPUT_DEFAULT_PROMPT;
}

const char *terminalInputGetPrompt(void)
{
  return mTerminalInputPrompt;
}

void terminalInputSetBuffer(char *pBuffer, int pSize)
{
  if (NULL != pBuffer)
  {
    mTerminalInputBuffer     = pBuffer;
    mTerminalInputBufferSize = pSize;
  }
  else
  {
    mTerminalInputBuffer     = mTerminalInputDefaultBuffer;
    mTerminalInputBufferSize = TERMINAL_INPUT_BUFFER_SIZE;
  }
}

int terminalInputEmpty(void)
{
  int result = 0 == mTerminalInputCursor;
  return result;
}

void terminalInputPromptDisplay(void)
{
  printf("%s %s", mTerminalInputPrompt, mTerminalInputBuffer);
  fflush(stdout);
}

void terminalInputPromptDisplayUnlessEmpty(void)
{
  if (!terminalInputEmpty())
  {
    terminalInputPromptDisplay();
  }
}

void terminalInputPrompt(void)
{
  terminalInputReset();
  terminalInputPromptDisplay();
}

int terminalInputFileReady(int pFile)
{
  struct timeval timeout;
  fd_set         fd_set;
  FD_ZERO(&fd_set);
  FD_SET(pFile, &fd_set);
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;
  int result = select(pFile + 1, &fd_set, NULL, NULL, &timeout) == 1 ? 1 : 0;
  return result;
}

int terminalInputReady(void)
{
  return terminalInputFileReady(STDIN_FILENO);
}

void terminalInputBackspace(void)
{
  if (0 < mTerminalInputCursor)
  {
    mTerminalInputCursor--;
    mTerminalInputBuffer[mTerminalInputCursor] = '\0';
  }
}

void terminalInputBufferCharacter(int pC)
{
  if (('\b' == pC) || (127 == pC) || (224 == pC))
  {
    terminalInputBackspace();
  }
  if (mTerminalInputCursor < TERMINAL_INPUT_BUFFER_SIZE - 1)
  {
    mTerminalInputBuffer[mTerminalInputCursor] = pC;
    mTerminalInputCursor++;
  }
}

const char *terminalInputGetBuffer(void)
{
  return mTerminalInputBuffer;
}


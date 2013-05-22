/***************************************************************
 *
 * terminalInput.c
 * This modules handles nonblocking terminal input.
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
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

// module headers
#include "error.h"
#include "terminalInput.h"

// module variables
char           mTerminalInputDefaultBuffer[TERMINAL_INPUT_BUFFER_SIZE];
char *         mTerminalInputBuffer;
int            mTerminalInputBufferSize;
const char *   mTerminalInputPrompt;
int            mTerminalInputCursor;
struct termios mTerminalInputOriginalSettings;

// initialize terminal input and enable nonblocking input
void terminalInputInit(const char *pPrompt, char *pBuffer, int pSize)
{
  struct termios newSettings;

  // set internal variable and clear state
  terminalInputSetPrompt(pPrompt);
  terminalInputSetBuffer(pBuffer, pSize);
  terminalInputReset();

  // get current terminal state
  if (tcgetattr(0, &mTerminalInputOriginalSettings) < 0)
  {
    FATAL_ERROR("Could not get terminal state during initialization.");
  }
  memcpy(&newSettings, &mTerminalInputOriginalSettings, sizeof(struct termios));

  // enable non-blocking terminal input
  newSettings.c_lflag &= ~ICANON;
  if (tcsetattr(0, TCSANOW, &newSettings) < 0)
  {
    FATAL_ERROR("Could not enable non-blocking terminal input.");
  }
}

// cleanup and restore terminal state
void terminalInputCleanUp(void)
{
  // restore terminal state
  if (tcsetattr(0, TCSANOW, &mTerminalInputOriginalSettings) < 0)
  {
    FATAL_ERROR("Could not restore terminal state.");
  }
}

// reset module state
void terminalInputReset(void)
{
  bzero(mTerminalInputBuffer, mTerminalInputBufferSize);
  mTerminalInputCursor = 0;
}

// set prompt
void terminalInputSetPrompt(const char *pPrompt)
{
  mTerminalInputPrompt = pPrompt ? pPrompt : TERMINAL_INPUT_DEFAULT_PROMPT;
}

// get prompt
const char *terminalInputGetPrompt(void)
{
  return mTerminalInputPrompt;
}

// set buffer
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

// get buffer
const char *terminalInputGetBuffer(void)
{
  return mTerminalInputBuffer;
}

// is buffer empty?
int terminalInputEmpty(void)
{
  int result = 0 == mTerminalInputCursor;
  return result;
}

// display prompt
void terminalInputPromptDisplay(void)
{
  printf("%s %s", mTerminalInputPrompt, mTerminalInputBuffer);
  fflush(stdout);
}

// display prompt unless it is empty
void terminalInputPromptDisplayUnlessEmpty(void)
{
  if (!terminalInputEmpty())
  {
    terminalInputPromptDisplay();
  }
}

// display new prompt, clear internal buffer first
void terminalInputPrompt(void)
{
  terminalInputReset();
  terminalInputPromptDisplay();
}

// is input file ready to read from?
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

// is stdin ready to read from?
int terminalInputReady(void)
{
  return terminalInputFileReady(STDIN_FILENO);
}

// delete a character from the buffer
void terminalInputBackspace(void)
{
  if (0 < mTerminalInputCursor)
  {
    mTerminalInputCursor--;
    mTerminalInputBuffer[mTerminalInputCursor] = '\0';
  }
}

// add a character to the buffer
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


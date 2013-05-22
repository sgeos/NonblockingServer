/***************************************************************
 *
 * terminalInput.h
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

#ifndef TERMINAL_INPUT_H
#define TERMINAL_INPUT_H

// required headers
#include "network.h"

// module macros
#define TERMINAL_INPUT_DEFAULT_PROMPT   ">"
#define TERMINAL_INPUT_BUFFER_SIZE      NETWORK_COMMUNICATION_BUFFER_SIZE

// function prototypes
void         terminalInputInit                     (const char *pPrompt, char *pBuffer, int pSize);
void         terminalInputCleanUp                  (void);
void         terminalInputReset                    (void);
void         terminalInputSetPrompt                (const char *pPrompt);
const char * terminalInputGetPrompt                (void);
void         terminalInputSetBuffer                (char *pBuffer, int pSize);
const char * terminalInputGetBuffer                (void);
int          terminalInputEmpty                    (void);
void         terminalInputPromptDisplay            (void);
void         terminalInputPromptDisplayUnlessEmpty (void);
void         terminalInputPrompt                   (void);
int          terminalInputFileReady                (int pFile);
int          terminalInputReady                    (void);
void         terminalInputBackspace                (void);
void         terminalInputBufferCharacter          (int pC);

#endif // TERMINAL_INPUT_H


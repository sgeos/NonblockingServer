/***************************************************************
 *
 * database.c
 * This module logs client communications in an SQLite database.
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

// module headers
#include "database.h"
#include "error.h"

// module variables
database_state_t mState;

// get library state
database_state_t *database_getState(void)
{
  return &mState;
}

// reset statement
void database_resetStatement(sqlite3_stmt *pStatement)
{
  int status;

  status = sqlite3_reset(pStatement);
  if (SQLITE_OK != status)
  {
    fatalError("Error reseting SQL statement!", 1);
  }
  status = sqlite3_clear_bindings(pStatement);
  if (SQLITE_OK != status)
  {
    fatalError("Error clearing SQL statement bindings!", 1);
  }
}

// add message
void database_addMessage(const char * pUser, const char * pMessage)
{
  // local variables
  database_state_t *state     = database_getState();
  sqlite3_stmt     *statement = state->addMessage;
  int               status;

  // bind values
  status = sqlite3_bind_text(statement, 1, pUser, strlen(pUser) + 1, SQLITE_STATIC);
  if ( SQLITE_OK != status )
  {
    fatalError("Error binding SQL statement!", 1);
  }
  status = sqlite3_bind_text(statement, 2, pMessage, strlen(pMessage) + 1, SQLITE_STATIC);
  if (SQLITE_OK != status)
  {
    fatalError("Error binding SQL statement!", 1);
  }

  // execute statement
  for (status = sqlite3_step(statement); SQLITE_DONE != status; status = sqlite3_step(statement))
  {
    if (SQLITE_ROW != status)
    {
      fatalError ("SQL command failed!", 1);
    }
  }
  database_resetStatement(statement);
}

// read file
int database_loadFile(database_state_t *pState, const char * pFileName)
{
  // local variables
  sqlite3 * database = pState->database;
  FILE    * file     = fopen(pFileName, "rb");
  int       result;

  if (file == 0)
  {
    error("SQL file failed to load!");
    return 1;
  }
  else
  {
    char * errorMessage;
    char   buffer[DATABASE_FILE_BUFFER_SIZE];
    char * line;
    int    offset;
    int    size;

    offset = 0;
    line   = &buffer[offset];
    size   = sizeof(buffer) - offset;
    while (NULL != fgets(line, size, file))
    {
      switch (sqlite3_complete(buffer))
      {
        // SQLite3 memory allocation failed
        case SQLITE_NOMEM:
          error("No memory!  SQL file failed to load!");
          fclose(file);
          return 1;
          break;

        // incomplete statement, append next line
        case 0:
          if ('-' == line[0] && '-' == line[1])
          {
            // comment, ignore line
            // this comment detection is not robust!
          }
          else
          {
            // read next line
            offset = strlen(buffer);
          }
          break;

        // complete statement, execute SQL and reset offset
        default:
          if (SQLITE_OK != sqlite3_exec(database, buffer, NULL, NULL, &errorMessage))
          {
            error(errorMessage);
            sqlite3_free(errorMessage);
          }
          offset = 0;
        break;
      }

      // update based on offset
      line   = &buffer[offset];
      size   = sizeof(buffer) - offset;
      if (size <= 1)
      {
        error("Buffer too small!  SQL file failed to load!");
        fclose(file);
        return 1;
      }
    }
    fclose(file);
  }
  return result;
}

// prepare statements
void database_initStatements(database_state_t *pState)
{
  // local variables
  database_state_t * state        = database_getState();
  sqlite3_stmt **    statement [] =
  {
    &state->userNew,
    &state->userLogin,
    &state->addMessage,
    &state->getLog
  };
  char *             sql [] =
  {
    DATABASE_STMT_USER_NEW,
    DATABASE_STMT_USER_LOGIN,
    DATABASE_STMT_ADD_MESSAGE,
    DATABASE_STMT_GET_LOG
  };
  int                size = sizeof(sql) / sizeof(char *);
  int                status;
  int                i;

  // prepare statements
  for (i = 0; i < size; i++)
  {
    status = sqlite3_prepare_v2(state->database, sql[i], strlen(sql[i]) + 1, statement[i], NULL);
    if (SQLITE_OK != status)
    {
      error     (sql[i]);
      fatalError("Error preparing SQL statement!", 1);
    }
  }
}

// initialize SQLite3
void database_init (void)
{
  // local variables
  database_state_t *state = database_getState();
  int status;

  // use thread safe serialized mode
  status = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
  if (SQLITE_OK != status)
  {
    fatalError("Thread-safe SQLite3 not supported!", 1);
  }

  // initialize SQLite3
  status = sqlite3_initialize();
  if (SQLITE_OK != status)
  {
    fatalError("Failed to initialize SQLite3!", 1);
  }

  // open database
  status = sqlite3_open(DATABASE_FILE, &state->database);
  if (SQLITE_OK != status)
  {
    fatalError("Failed to open SQLite3 database!", 1);
  }

  // initialize database
  database_loadFile(state, DATABASE_SQL);

  // prepare statements
  database_initStatements(state);
}

// close SQLite3
void database_close(void)
{
  // local variables
  database_state_t *state = database_getState();

  // finalize prepared statements
  sqlite3_finalize(state->userNew   );
  sqlite3_finalize(state->userLogin );
  sqlite3_finalize(state->addMessage);
  sqlite3_finalize(state->getLog    );

  // close database
  sqlite3_close(state->database);
}


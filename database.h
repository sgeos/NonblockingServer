/***************************************************************
 *
 * database.h
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

#ifndef DATABASE_H
#define DATABASE_H

// module headers
#include "sqlite3.h"

// literal constants
#define DATABASE_FILE             "log.db"
#define DATABASE_SQL              "log.sql"
#define DATABASE_FILE_BUFFER_SIZE 256

// prepared statements
#define DATABASE_STMT_USER_NEW \
  "INSERT INTO user_list ( id, user, time ) SELECT COALESCE ( MAX ( id ) + 1, 1 ), 'User_' || COALESCE ( MAX ( id ) + 1, 1 ), DATETIME ( 'NOW' ) FROM user_list; SELECT user FROM user_list ORDER BY id DESC LIMIT 1;"
#define DATABASE_STMT_USER_LOGIN \
  "INSERT OR REPLACE INTO user_list ( id, user, time ) VALUES ( COALESCE ( (SELECT id FROM user_list WHERE user=?1), (SELECT MAX ( id ) + 1 FROM user_list), 1 ), ?1, DATETIME ( 'NOW' ));"
#define DATABASE_STMT_ADD_MESSAGE \
  "INSERT INTO message_log ( id, user, message, time ) SELECT COALESCE ( MAX ( id ) + 1, 1 ), ?1, ?2, DATETIME ( 'NOW' ) FROM message_log;"
#define DATABASE_STMT_GET_LOG \
  "SELECT * FROM message_log ORDER BY time DESC LIMIT ?1;"

// types
typedef struct database_state_t
{
  sqlite3      * database;
  sqlite3_stmt * userNew;
  sqlite3_stmt * userLogin;
  sqlite3_stmt * addMessage;
  sqlite3_stmt * getLog;
} database_state_t;

// function prototypes
void database_init       ( void );
void database_close      ( void );
void database_addMessage ( const char * pUser, const char * pMessage );

#endif // DATABASE_H


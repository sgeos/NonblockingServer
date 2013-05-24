----------------------------------------------------------------
--
-- log.sql
-- SQL schema for client communications log.
--
----------------------------------------------------------------
--
-- This software was written in 2013 by the following author(s):
-- Brendan A R Sechter <bsechter@sennue.com>
--
-- To the extent possible under law, the author(s) have
-- dedicated all copyright and related and neighboring rights
-- to this software to the public domain worldwide. This
-- software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain
-- Dedication along with this software. If not, see
-- <http://creativecommons.org/publicdomain/zero/1.0/>.
--
-- Please release derivative works under the terms of the CC0
-- Public Domain Dedication.
-- 
----------------------------------------------------------------

CREATE TABLE IF NOT EXISTS user_list
(
  id      INTEGER,
  user    VARCHAR(32),
  time    DATE,
  PRIMARY KEY ( user )
);

CREATE TABLE IF NOT EXISTS message_log
(
  id      INTEGER,
  user    VARCHAR(32),
  message VARCHAR(256),
  time    DATE,
  PRIMARY KEY ( id )
);

-- Login New User
-- INSERT INTO user_list ( id, user, time ) SELECT COALESCE ( MAX ( id ) + 1, 1 ), 'User_' || COALESCE ( MAX ( id ) + 1, 1 ), DATETIME ( 'NOW' ) FROM user_list;

-- Login Existing User
-- INSERT OR REPLACE INTO user_list ( id, user, time )
--   VALUES (
--     COALESCE ( (SELECT id FROM user_list WHERE user='MyUserName'), (SELECT MAX ( id ) + 1 FROM user_list), 1 ),
--     'MyUserName',
--     DATETIME ( 'NOW' )
--   );

-- New Message
-- INSERT INTO message_log ( id, user, message, time ) SELECT COALESCE ( MAX ( id ) + 1, 1 ), 'MyUserName', 'This is my message.', DATETIME ( 'NOW' ) FROM message_log;

-- Message Log
-- SELECT * FROM message_log ORDER BY time DESC LIMIT 50;


################################################################
#
# server.py
# A minimalistic chat server written in Python using Twisted.
# python server.py
#
################################################################
#
# This software was written in 2013 by the following author(s):
# Brendan A R Sechter <bsechter@sennue.com>
#
# To the extent possible under law, the author(s) have
# dedicated all copyright and related and neighboring rights
# to this software to the public domain worldwide. This
# software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain
# Dedication along with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.
#
# Please release derivative works under the terms of the CC0
# Public Domain Dedication.
#
################################################################

from twisted.internet.protocol import Factory, Protocol
from twisted.internet import reactor

DEFAULT_PORT = 51717
 
class simplePythonChatServer(Protocol):
  def connectionMade(self):
    print "Client Connected : ", self
    self.factory.clients.append(self)
    self.name = "Guest User"
 
  def connectionLost(self, reason):
    print "Client Disconnected : ", self, " : ", reason
    self.factory.clients.remove(self)

  def dataReceived(self, data):
    a = data.rstrip('\r\n').split('/')
    print a
    if 3 < len(a):
      command = a[1]
      user    = a[2]
      content = a[3]
      msg = ""
      if command == "iam":
        self.name = content
        msg = "New User : " + self.name
      elif command == "msg":
        msg = self.name + " : " + content
      elif command == "exit":
        msg = "Disconnected : " + self.name
        self.transport.loseConnection()
      elif command == "die":
        msg = "Simple Python chat server stopped."
        reactor.stop()
      print msg
      for client in self.factory.clients:
        client.message(msg)

  def message(self, message):
    self.transport.write(message + '\n')

factory = Factory()
factory.protocol = simplePythonChatServer
factory.clients = []
reactor.listenTCP(DEFAULT_PORT, factory)
print "Simple Python chat server started."
print "Port : ", DEFAULT_PORT
reactor.run()


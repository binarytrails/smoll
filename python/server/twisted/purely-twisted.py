import sys
from twisted.python import log
from twisted.internet import reactor
from twisted.internet.protocol import ServerFactory, ClientFactory, Protocol

class EchoServerProtocol(Protocol):
    def dataReceived(self, data):
        log.msg('Data received %s' % data)
        self.transport.write(b'Server push back: %s' % data)

    def connectionMade(self):
        log.msg('Client connection from %s' % self.transport.getPeer())

    def connectionLost(self, reason):
        log.msg('Lost connection because %s' % reason)

class EchoServerFactory(ServerFactory):
    def buildProtocol(self, addr):
        return EchoServerProtocol()

if __name__ == '__main__':
    log.startLogging(sys.stdout)
    log.msg('Starting twisted engines...')

    server = EchoServerFactory()
    reactor.listenTCP(8080, server)
    
    log.msg('Listening on http://127.0.0.1:8080')
    reactor.run()

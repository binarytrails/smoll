import cherrypy

class User(object):
    
    @cherrypy.expose
    def index(self):
        return "Welcome user!"
    
    @cherrypy.expose
    def details(self, user):
        return "%s, you have no details." % user

class Root(object):
    user = User()

    @cherrypy.expose
    def index(self):
        return "Rooting?"

cherrypy.tree.mount(Root())
cherrypy.server.unsubscribe()

server2 = cherrypy._cpserver.Server()
server2.socket_port=8080
server2._socket_host='127.0.0.1'
server2.thread_pool=10
server2.subscribe()

cherrypy.engine.start()
cherrypy.engine.block()

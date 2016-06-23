from klein import run, route
from twisted.web.resource import Resource

class User(Resource, object):
    def __init__(self, user_id):
        super(User, self).__init__()
        self.user_id = user_id

    def render_GET(self, request):
        print('GET')
        return request        

    def render_POST(self, request):
        print('POST')
        return request

    def render_PUT(self, request):
        print('PUT')
        return request

@route('/users/<int:user_id>')
def user(request, user_id):
    return User(user_id)

run("localhost", 8080)
# curl -X METHOD http://127.0.0.1:8080/users/1

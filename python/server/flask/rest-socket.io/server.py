import json

from flask import Flask
from flask_restful import Resource, Api
from flask_socketio import SocketIO, send, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 't0p_s3cr3t'
api = Api(app)
socketio = SocketIO(app)

class Chat(Resource):
    def get(self):
        return json.dumps('GET')

    def post(self):
        return json.dumps('POST')
    
    def put(self):
        return json.dumps('PUT')

    def delete(self):
        return json.dumps('DELETE')

@socketio.on('message', namespace='/chat')
def message(data):
    emit('reply', data)

api.add_resource(Chat, '/chat')

if __name__ == '__main__':
    socketio.run(app, port=8000)


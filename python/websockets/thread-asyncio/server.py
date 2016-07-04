import time
import threading
import asyncio
import websockets
from websockets import exceptions as ws_ex

class ChatServer:
    q = asyncio.Queue()

    def __init__(self):
        self.websockets = list()

    async def serve(self, websocket, path):
 
        if (websocket not in self.websockets):
            self.websockets.append(websocket)
            print('server: adding new socket: %s' % str(websocket))

        print('server: sending "welcome" to %s' % str(websocket))
        await websocket.send('welcome')

    async def notify(self):
        while True:
            message = await self.q.get()
            print('server: got "%s"' % message)

            for websocket in self.websockets:
                print('server: sending "%s" to %s' % (message, websocket))
                try:
                    await websocket.send(message)
                except ws_ex.ConnectionClosed:
                    print('server: connection closed to %s' % websocket)
    
def callback_simulation(loop, chat):
    while True:
        time.sleep(3)
        print('callback: adding "hi" to queue of size %s' % chat.q.qsize())
        loop.call_soon_threadsafe(chat.q.put_nowait, 'hi')

loop = asyncio.get_event_loop()
chat = ChatServer()

chat_server = websockets.serve(chat.serve, '127.0.0.1', 5678)

loop.create_task(chat_server)
loop.create_task(chat.notify())

t = threading.Thread(target=callback_simulation, args=(loop, chat,))
t.start()

loop.run_forever()
    

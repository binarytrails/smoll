import time
import threading
import asyncio
import websockets
from websockets import exceptions as ws_ex

class ChatServer:
    messages = asyncio.Queue()

    def __init__(self):
        self.websockets = list()

    async def serve(self, websocket, path):
        if (websocket not in self.websockets):
            self.websockets.append(websocket)
            print('server: adding new socket: %s' % str(websocket))

        print('server: sending "welcome" to %s' % str(websocket))
        await websocket.send('welcome')

        while True:
            # keeps the websocket alive by the current design
            # see: https://github.com/aaugustin/websockets/issues/122
            await asyncio.sleep(60)
            if (websocket not in self.websockets):
                print('server: closing websocket %s' % websocket)
                break

    async def notify(self):
        while True:
            message = await self.messages.get()
            print('server: got "%s"' % message)

            for websocket in self.websockets:
                print('server: sending "%s" to %s' % (message, websocket))
                try:
                    await websocket.send(message)
                except ws_ex.ConnectionClosed:
                    self.websockets.remove(websocket)
                    print('server: connection closed to %s' % websocket)

    
def callback_simulation(loop, chat):
    while True:
        time.sleep(3)
        message = 'hi from callback'
        print('callback: adding "%s" to queue of size %s' % (
            message, chat.messages.qsize(),
        ))
        loop.call_soon_threadsafe(chat.messages.put_nowait, message)

loop = asyncio.get_event_loop()
chat = ChatServer()

chat_server = websockets.serve(chat.serve, '127.0.0.1', 5678)

loop.create_task(chat_server)
loop.create_task(chat.notify())

t = threading.Thread(target=callback_simulation, args=(loop, chat,))
t.start()

loop.run_forever()
    

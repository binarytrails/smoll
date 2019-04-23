// Vsevolod Ivanov

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dyad.h"

#include <thread>
#include <opendht.h>

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();
    private:
        dht::DhtRunner *node;
        dyad_Stream *dyadStream;

        static void onLine(dyad_Event *event)
        {
            char path[128];

            if (sscanf(event->data, "GET %127s", path) == 1)
            {
                printf("%s %s\n", dyad_getAddress(event->stream), path);
                dyad_writef(event->stream, "HTTP/1.1 200 OK\r\n\r\n");
                dyad_writef(event->stream, "%s", "dht::get()");
            }

            dyad_end(event->stream);
        }
        static void onAccept(dyad_Event *event)
        {
          dyad_addListener(event->remote, DYAD_EVENT_LINE, onLine, NULL);
        }
        static void onListen(dyad_Event *event)
        {
          printf("http://localhost:%d\n", dyad_getPort(event->stream));
        }
        static void onError(dyad_Event *event)
        {
          printf("server error: %s\n", event->msg);
        }

};

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

        void onAccept(dyad_Event *event);
        void onListen(dyad_Event *event);
        void onLine(dyad_Event *event);
        void onError(dyad_Event *event);
};

// Vsevolod Ivanov

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dyad.h"

class HttpServer
{
    public:
        HttpServer();
        ~HttpServer();
    private:
        dyad_Stream *dyadStream;

        void onAccept(dyad_Event *event);
        void onListen(dyad_Event *event);
        void onData(dyad_Event *event);
        void onLine(dyad_Event *event);
        void onError(dyad_Event *event);
};

// Vsevolod Ivanov

#include "proxyserv_dyad.h"

DhtProxyServer::DhtProxyServer()
{
    this->node = new dht::DhtRunner();
    this->node->run(42222, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    dyad_init();
    this->dyadStream = dyad_newStream();
    dyad_addListener(this->dyadStream, DYAD_EVENT_ERROR, [](dyad_Event* event){
        ((DhtProxyServer*)event->udata)->onError(event);
    }, this);
    dyad_addListener(this->dyadStream, DYAD_EVENT_ACCEPT, [](dyad_Event* event){
        ((DhtProxyServer*)event->udata)->onAccept(event);
    }, this);
    dyad_addListener(this->dyadStream, DYAD_EVENT_LISTEN, [](dyad_Event* event){
        ((DhtProxyServer*)event->udata)->onListen(event);
    }, this);
    dyad_listen(this->dyadStream, 8080);
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
    delete this->dyadStream;
    delete this->node;
}

void DhtProxyServer::onAccept(dyad_Event *event)
{
    dyad_addListener(event->remote, DYAD_EVENT_LINE, [](dyad_Event* event){
        ((DhtProxyServer*)event->udata)->onLine(event);
    }, this);
}

void DhtProxyServer::onListen(dyad_Event *event)
{
    printf("http://localhost:%d\n", dyad_getPort(event->stream));
}

void DhtProxyServer::onLine(dyad_Event *event)
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

void DhtProxyServer::onError(dyad_Event *event)
{
    printf("server error: %s\n", event->msg);
}

int main()
{
    DhtProxyServer proxy;
    while (dyad_getStreamCount() > 0)
    {
        dyad_update();
    }
    return 0;
}

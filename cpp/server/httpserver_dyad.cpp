// Vsevolod Ivanov

#include "httpserver_dyad.h"

HttpServer::HttpServer()
{
    dyad_init();
    this->dyadStream = dyad_newStream();
    dyad_addListener(this->dyadStream, DYAD_EVENT_ERROR, [](dyad_Event* event){
        ((HttpServer*)event->udata)->onError(event);
    }, this);
    dyad_addListener(this->dyadStream, DYAD_EVENT_ACCEPT, [](dyad_Event* event){
        ((HttpServer*)event->udata)->onAccept(event);
    }, this);
    dyad_addListener(this->dyadStream, DYAD_EVENT_LISTEN, [](dyad_Event* event){
        ((HttpServer*)event->udata)->onListen(event);
    }, this);
    dyad_listen(this->dyadStream, 8080);
}

HttpServer::~HttpServer()
{
    delete this->dyadStream;
}

void HttpServer::onAccept(dyad_Event *event)
{
    printf("\nonAccept: %s", event->data);
    /*
    dyad_addListener(event->remote, DYAD_EVENT_DATA, [](dyad_Event* event){
        ((HttpServer*)event->udata)->onData(event);
    }, this);
    */
    dyad_addListener(event->remote, DYAD_EVENT_LINE, [](dyad_Event* event){
        ((HttpServer*)event->udata)->onLine(event);
    }, this);
}

void HttpServer::onListen(dyad_Event *event)
{
    printf("http://localhost:%d\n", dyad_getPort(event->stream));
}

void HttpServer::onData(dyad_Event *event)
{
    printf("\nonData: %s", event->data);
    //TODO HTTP parsing
    dyad_end(event->stream);
}

void HttpServer::onLine(dyad_Event *event)
{
    printf("\nonLine: %s", event->data);
    char path[128];

    if (sscanf(event->data, "GET %127s", path) == 1)
    {
        printf("%s %s\n", dyad_getAddress(event->stream), path);
        dyad_writef(event->stream, "HTTP/1.1 200 OK\r\n\r\n");
        dyad_writef(event->stream, "%s", "dht::get()");
    }
    else if (sscanf(event->data, "PUT %127s", path) == 1)
    {
        printf("here: %s %s %i\n", event->stream, path, sizeof(event->data));
        dyad_writef(event->stream, "HTTP/1.1 200 OK\r\n\r\n");
    }
}

void HttpServer::onError(dyad_Event *event)
{
    printf("server error: %s\n", event->msg);
}

int main()
{
    HttpServer proxy;
    while (dyad_getStreamCount() > 0)
    {
        dyad_update();
    }
    return 0;
}

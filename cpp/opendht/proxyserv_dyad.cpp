// Vsevolod Ivanov

#include "proxyserv_dyad.h"

DhtProxyServer::DhtProxyServer()
{
    this->node = new dht::DhtRunner();
    this->node->run(42222, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    dyad_init();
    this->dyadStream = dyad_newStream();
    dyad_addListener(this->dyadStream, DYAD_EVENT_ERROR,  this->onError,  NULL);
    dyad_addListener(this->dyadStream, DYAD_EVENT_ACCEPT, this->onAccept, NULL);
    dyad_addListener(this->dyadStream, DYAD_EVENT_LISTEN, this->onListen, NULL);
    dyad_listen(this->dyadStream, 8080);
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
    delete this->dyadStream;
    delete this->node;
}

int main(void)
{
  DhtProxyServer proxy;
  while (dyad_getStreamCount() > 0)
  {
    dyad_update();
  }
  return 0;
}

// Vsevolod Ivanov

#include "dhtproxyserver.h"

DhtProxyServer::DhtProxyServer() : node(new dht::DhtRunner)
{
    this->node->run(42662, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    Pistache::Address restServerAddr(Pistache::Ipv4::any(), Pistache::Port(9080));
    auto opts = Pistache::Http::Endpoint::options()
        .threads(1)
        .flags(Pistache::Tcp::Options::InstallSignalHandler);
    this->restServer.reset(new Pistache::Http::Endpoint(restServerAddr));
    this->restServer->init(opts);

    Pistache::Rest::Routes::Get(this->restRouter, "/:name",
        Pistache::Rest::Routes::bind(&DhtProxyServer::get, this));
    Pistache::Rest::Routes::Put(this->restRouter, "/:name",
        Pistache::Rest::Routes::bind(&DhtProxyServer::put, this));

    this->restServer->setHandler(this->restRouter.handler());
}

DhtProxyServer::~DhtProxyServer()
{
    this->restServer->shutdown();
    this->node->join();
}
void DhtProxyServer::run()
{
    this->restServer->serve();
}

void DhtProxyServer::get(const Pistache::Http::Request& request,
                         Pistache::Http::ResponseWriter response)
{
    response.send(Pistache::Http::Code::Ok, "Getting node data");
}

void DhtProxyServer::put(const Pistache::Http::Request& request,
                          Pistache::Http::ResponseWriter response)
{
    response.send(Pistache::Http::Code::Ok, request.body(), MIME(Text, Plain));
}

int main(int argc, char *argv[])
{
    DhtProxyServer dhtProxy;
    dhtProxy.run();
}

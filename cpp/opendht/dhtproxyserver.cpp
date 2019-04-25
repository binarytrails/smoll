// Vsevolod Ivanov

#include "dhtproxyserver.h"

DhtProxyServer::DhtProxyServer()
{
    this->node = new dht::DhtRunner();
    //this->node->run(42222, dht::crypto::generateIdentity(), true);
    //this->node->bootstrap("bootstrap.jami.net", "4222");
}

DhtProxyServer::~DhtProxyServer()
{
    this->restServer->shutdown();
    delete this->restServer;

    this->node->join();
    delete this->node;
}
void DhtProxyServer::run()
{
    Pistache::Address restServerAddr(Pistache::Ipv4::any(), Pistache::Port(9080));
    auto opts = Pistache::Http::Endpoint::options().threads(1);
    this->restServer = new Pistache::Http::Endpoint(restServerAddr);
    this->restServer->init(opts);
    this->restServer->setHandler(Pistache::Http::make_handler<DhtProxyServer>());
    this->restServer->serve();
}
void DhtProxyServer::onRequest(const Pistache::Http::Request& request,
                               Pistache::Http::ResponseWriter response)
{
    if (request.resource() == "/node")
    {
        if (request.method() == Pistache::Http::Method::Get)
        {
            response.send(Pistache::Http::Code::Ok, "Getting node data");
        }
        else if (request.method() == Pistache::Http::Method::Put)
        {
            response.send(Pistache::Http::Code::Ok, request.body(), MIME(Text, Plain));
        }
    }
    else
    {
        response.send(Pistache::Http::Code::Not_Found);
    }
}

int main(int argc, char *argv[])
{
    DhtProxyServer *dhtProxy = new DhtProxyServer();
    dhtProxy->run();
    delete dhtProxy;
}

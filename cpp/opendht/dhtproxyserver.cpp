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
    this->restServer->setHandler(
        Pistache::Http::make_handler<DhtProxyServer::HttpHandler>());
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
void DhtProxyServer::HttpHandler::onRequest(
    const Pistache::Http::Request& request,
    Pistache::Http::ResponseWriter response
){
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
    DhtProxyServer dhtProxy;
    dhtProxy.run();
}

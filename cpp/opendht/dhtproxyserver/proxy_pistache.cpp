// Vsevolod Ivanov

#include "proxy_pistache.h"

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

    Pistache::Rest::Routes::Get(this->restRouter, "/:hash",
        Pistache::Rest::Routes::bind(&DhtProxyServer::get, this));
    Pistache::Rest::Routes::Put(this->restRouter, "/:hash",
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

void DhtProxyServer::get(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response)
{
    auto hash = request.param(":hash").as<std::string>();
    dht::InfoHash infoHash(hash);
    if (!infoHash)
        infoHash = dht::InfoHash::get(hash);

    // TODO more efficient example using get() callbacks
    //      however, it needs kept alive pistache session
    auto values = node->get(infoHash).get();

    dht::Blob blob;
    std::string data;
    for (std::vector<std::shared_ptr<dht::Value>>::iterator it = values.begin();
         it != values.end(); ++it
    ){
        blob = (*it)->data;
        std::string value = std::string(blob.begin(), blob.end());
        data.append(value);
        data.append(",");
    }
    response.send(Pistache::Http::Code::Ok, data.c_str());
}

void DhtProxyServer::put(const Pistache::Rest::Request& request,
                          Pistache::Http::ResponseWriter response)
{
    response.send(Pistache::Http::Code::Ok, request.body(), MIME(Text, Plain));
}

int main(int argc, char *argv[])
{
    DhtProxyServer dhtProxy;
    dhtProxy.run();
}

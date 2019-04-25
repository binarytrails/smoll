// Vsevolod Ivanov

#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <opendht.h>
#include <json/json.h>

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        void run();

        void get(const Pistache::Http::Request& request,
                       Pistache::Http::ResponseWriter response);
        void put(const Pistache::Http::Request& request,
                       Pistache::Http::ResponseWriter response);

        std::shared_ptr<dht::DhtRunner> node;
        std::unique_ptr<Pistache::Http::Endpoint> restServer;
        Pistache::Rest::Router restRouter;
};

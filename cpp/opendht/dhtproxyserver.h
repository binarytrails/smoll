// Vsevolod Ivanov

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include <opendht.h>
#include <string>
#include <chrono>
#include <condition_variable>
//#include <json/json.h>

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        void run();

        void get(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response);
        void put(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response);

        std::shared_ptr<dht::DhtRunner> node;
        std::unique_ptr<Pistache::Http::Endpoint> restServer;
        Pistache::Rest::Router restRouter;
};

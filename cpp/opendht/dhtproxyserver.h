// Vsevolod Ivanov

#include <pistache/endpoint.h>
#include <opendht.h>

class DhtProxyServer : public Pistache::Http::Handler
{
    public:
        HTTP_PROTOTYPE(DhtProxyServer)

        DhtProxyServer();
        ~DhtProxyServer();

        void run();
        void onRequest(const Pistache::Http::Request& request,
                       Pistache::Http::ResponseWriter response) override;

    private:
        dht::DhtRunner *node;
        Pistache::Http::Endpoint *restServer;
};

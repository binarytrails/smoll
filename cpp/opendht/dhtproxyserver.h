// Vsevolod Ivanov

#include <pistache/endpoint.h>
#include <opendht.h>

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        void run();

    private:
        class HttpHandler : public Pistache::Http::Handler
        {
            public:
                HTTP_PROTOTYPE(HttpHandler)

                void onRequest(const Pistache::Http::Request& request,
                               Pistache::Http::ResponseWriter response) override;
        };

        std::shared_ptr<dht::DhtRunner> node;
        std::unique_ptr<Pistache::Http::Endpoint> restServer;
};

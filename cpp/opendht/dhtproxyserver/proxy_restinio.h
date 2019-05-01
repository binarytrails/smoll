// Vsevolod Ivanov

#include <opendht.h>
#include <restinio/all.hpp>

using RestRouter = restinio::router::express_router_t<>;
using RestRouteTraits = restinio::traits_t<
    restinio::asio_timer_manager_t,
    restinio::single_threaded_ostream_logger_t,
    RestRouter>;

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        int run();

        restinio::request_handling_status_t
        get(restinio::request_handle_t request,
            restinio::router::route_params_t params);

    private:
        std::shared_ptr<dht::DhtRunner> node;

        std::unique_ptr<RestRouter> createRestRouter();
};

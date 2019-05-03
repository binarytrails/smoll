// Vsevolod Ivanov

#include <chrono>
#include <condition_variable>
#include <json/json.h>
#include <opendht.h>
#include <opendht/value.h>
#include <restinio/all.hpp>

using RestRouter = restinio::router::express_router_t<>;
using RestRouteTraits = restinio::traits_t<
    restinio::asio_timer_manager_t,
    restinio::single_threaded_ostream_logger_t,
    RestRouter>;
using request_status = restinio::request_handling_status_t;

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        int run();

        request_status get(restinio::request_handle_t request,
                           restinio::router::route_params_t params);
        request_status put(restinio::request_handle_t request,
                           restinio::router::route_params_t params);

    private:
        std::shared_ptr<dht::DhtRunner> node;
        Json::StreamWriterBuilder jsonBuilder;

        std::unique_ptr<RestRouter> createRestRouter();
};

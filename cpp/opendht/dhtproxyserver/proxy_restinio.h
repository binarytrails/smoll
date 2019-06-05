// Vsevolod Ivanov

#include <chrono>
#include <condition_variable>
#include <json/json.h>
#include <opendht.h>
#include <restinio/all.hpp>

using RestRouter = restinio::router::express_router_t<>;
using RestRouterTraits = restinio::traits_t<
    restinio::asio_timer_manager_t,
    restinio::single_threaded_ostream_logger_t,
    RestRouter>;
using ServerSettings = restinio::run_on_thread_pool_settings_t<RestRouterTraits>;
using request_status = restinio::request_handling_status_t;
using response_t = restinio::chunked_output_t;

class DhtProxyServer
{
    public:
        DhtProxyServer(std::shared_ptr<dht::DhtRunner> dhtNode,
                       in_port_t port = 8000);
        ~DhtProxyServer();

        bool running();
        void stop();

        request_status options(restinio::request_handle_t request,
                               restinio::router::route_params_t params);
        request_status getNodeInfo(restinio::request_handle_t request,
                                   restinio::router::route_params_t params);
        request_status get(restinio::request_handle_t request,
                           restinio::router::route_params_t params);
        request_status put(restinio::request_handle_t request,
                           restinio::router::route_params_t params);

    private:
        template <typename HttpResponse>
        HttpResponse initHttpResponse(HttpResponse response);
        typedef std::function<void(void)> response_func;

        std::unique_ptr<RestRouter> createRestRouter();

        std::shared_ptr<dht::DhtRunner> dhtNode;
        Json::StreamWriterBuilder jsonBuilder;

        std::thread serverThread {};
        restinio::http_server_t<RestRouterTraits> httpServer_;

        mutable std::mutex statsMutex;
        mutable dht::NodeInfo dhtNodeInfo {};
        mutable std::atomic<size_t> requestCount {0};

        const std::string RESP_MSG_MISSING_PARAMS = "{\"err\":\"Missing parameters\"}";
        const std::string RESP_MSG_PUT_FAILED = "{\"err\":\"Put failed\"}";
};

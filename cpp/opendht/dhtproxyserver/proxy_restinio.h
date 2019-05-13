// Vsevolod Ivanov

#include <chrono>
#include <condition_variable>
#include <json/json.h>
#include <opendht.h>
#include <restinio/all.hpp>
#include <so_5/all.hpp>

using RestRouter = restinio::router::express_router_t<>;
using RestRouterTraits = restinio::traits_t<
    restinio::asio_timer_manager_t,
    restinio::single_threaded_ostream_logger_t,
    RestRouter>;
using RequestStatus = restinio::request_handling_status_t;
using RequestOutput = restinio::chunked_output_t;
using Response = restinio::response_builder_t<RequestOutput>;

class DhtProxyServer
{
    public:
        DhtProxyServer(std::shared_ptr<dht::DhtRunner> dhtNode,
                       in_port_t port = 8000);
        ~DhtProxyServer();

        template <typename HttpResponse>
        HttpResponse initHttpResponse(HttpResponse response);

        void serverProcessing(so_5::mchain_t requestChannel);

        RequestStatus options(restinio::request_handle_t request,
                              restinio::router::route_params_t params);
        RequestStatus getNodeInfo(restinio::request_handle_t request,
                                  restinio::router::route_params_t params);
        RequestStatus get(restinio::request_handle_t request,
                          restinio::router::route_params_t params);
        RequestStatus put(restinio::request_handle_t request,
                          restinio::router::route_params_t params);

    private:
        const std::string RESP_MSG_MISSING_PARAMS = "{\"err\":\"Missing parameters\"}";
        const std::string RESP_MSG_PUT_FAILED = "{\"err\":\"Put failed\"}";

        std::shared_ptr<dht::DhtRunner> dhtNode;
        Json::StreamWriterBuilder jsonBuilder;
        std::thread serverThread {};
        std::thread serverProcessingThread;

        mutable std::mutex statsMutex;
        mutable dht::NodeInfo dhtNodeInfo {};
        mutable std::atomic<size_t> requestCount {0};

        // Message for transfering requests to serverProcessingThread
        struct HandleRequest
        {
            restinio::request_handle_t m_req;
        };
        // Message for delaying request processing
        struct TimeoutElapsed
        {
            Response m_resp;
            int m_counter;
        };
        so_5::mchain_t requestChannel;
        /* Launching SObjectizer on a separate thread
         * There is no need to start and shutdown SObjectize:
         * the wrapped_env_t instance does it automatically.*/
        so_5::wrapped_env_t sobj;

        std::unique_ptr<RestRouter> createRestRouter();
};

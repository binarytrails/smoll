// Vsevolod Ivanov

#include "proxy_restinio.h"

DhtProxyServer::DhtProxyServer(): node(new dht::DhtRunner)
{
    this->node->run(4444, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
}

std::unique_ptr<RestRouter> DhtProxyServer::createRestRouter()
{
    auto restRouter = std::make_unique<RestRouter>();

    using namespace std::placeholders;
    restRouter->http_get("/:hash",
                         std::bind(&DhtProxyServer::get, this, _1, _2));
    return restRouter;
}

int DhtProxyServer::run()
{
    using namespace std::chrono;
    try
    {
        restinio::run(
            restinio::on_this_thread<RestRouteTraits>()
                .address("127.0.0.1") .port(8080)
                .request_handler(this->createRestRouter())
                .read_next_http_message_timelimit(10s)
                .write_http_response_timelimit(1s)
                .handle_request_timeout(1s));
    }
    catch(const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

restinio::request_handling_status_t
DhtProxyServer::get(restinio::request_handle_t request,
                    restinio::router::route_params_t params)
{
    auto response = request->create_response();
    response.append_body(params["hash"]);
    return response.done();
}

int main(int argc, char* argv[])
{
    DhtProxyServer dhtproxy;
    int error_status = dhtproxy.run();
    return error_status;
}

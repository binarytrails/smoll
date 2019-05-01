// Vsevolod Ivanov

#include "proxy_restinio.h"

DhtProxyServer::DhtProxyServer():
    node(new dht::DhtRunner), restRouter(new RestRouter)
{
    this->node->run(4444, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    using namespace std::placeholders;
    this->restRouter->http_get("/:hash", std::bind(
                                &DhtProxyServer::get, this, _1, _2));
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
}

int DhtProxyServer::run()
{
    try
    {
        /*
        restinio::run(
            restinio::on_this_thread<RestRouteTraits>()
                .address("127.0.0.1")
                .request_handler(serverHandler) // TODO
                .read_next_http_message_timelimit(10)
                .write_http_response_timelimit(1)
                .handle_request_timeout(1));
        */
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
    return response.done();
}

int main(int argc, char* argv[])
{
    DhtProxyServer dhtproxy;
    int error_status = dhtproxy.run();
    return error_status;
}

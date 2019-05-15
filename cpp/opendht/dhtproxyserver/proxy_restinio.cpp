/* Vsevolod Ivanov
 *
 * TODO:
 *  - set connection timeout
 *  - implement sessions
 */

#include "proxy_restinio.h"

DhtProxyServer::DhtProxyServer(std::shared_ptr<dht::DhtRunner> dhtNode,
                               in_port_t port):
    dhtNode(dhtNode)
{
    this->jsonBuilder["commentStyle"] = "None";
    this->jsonBuilder["indentation"] = "";

    this->serverRespThread = std::thread(&DhtProxyServer::dispatchResponses, this);
    this->serverThread = std::thread([this, port](){
        using namespace std::chrono;
        auto maxThreads = std::thread::hardware_concurrency() - 2; //dht,resp
        auto restThreads = maxThreads > 1 ? maxThreads : 1;
        printf("Running on restinio on %i threads\n", restThreads);
        auto settings = restinio::on_thread_pool<RestRouterTraits>(restThreads);
        settings.address("0.0.0.0");
        settings.port(port);
        settings.request_handler(this->createRestRouter());
        settings.read_next_http_message_timelimit(10s);
        settings.write_http_response_timelimit(10s);
        settings.handle_request_timeout(10s);
        settings.socket_options_setter([](auto & options){
            options.set_option(asio::ip::tcp::no_delay{true});
        });
        try {
            restinio::run(std::move(settings));
        }
        catch(const std::exception &ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    });
}

DhtProxyServer::~DhtProxyServer()
{
    if (this->serverThread.joinable())
        this->dhtNode->join();

    if (this->serverRespThread.joinable())
    {
        std::unique_lock<std::mutex> lock(serverRespLock_);
        stopServer_ = true;
        lock.unlock();
        serverRespCv_.notify_all();
        this->serverRespThread.join();
    }
}

std::unique_ptr<RestRouter> DhtProxyServer::createRestRouter()
{
    using namespace std::placeholders;
    auto router = std::make_unique<RestRouter>();
    router->add_handler(restinio::http_method_t::http_options,
                        "/:hash", std::bind(&DhtProxyServer::options, this, _1, _2));
    router->http_get("/", std::bind(&DhtProxyServer::getNodeInfo, this, _1, _2));
    router->http_get("/:hash", std::bind(&DhtProxyServer::get, this, _1, _2));
    router->http_post("/:hash", std::bind(&DhtProxyServer::put, this, _1, _2));
    return router;
}

template <typename HttpResponse>
HttpResponse DhtProxyServer::initHttpResponse(HttpResponse response)
{
    response.append_header("Server", "RESTinio");
    response.append_header(restinio::http_field::content_type, "application/json");
    response.append_header(restinio::http_field::access_control_allow_origin, "*");
    response.connection_keep_alive();
    return response;
}

void DhtProxyServer::scheduleRespDispatch(response_func&& response)
{
    std::unique_lock<std::mutex> lock(serverRespLock_);
    serverRespQueue_.push(std::move(response));
    lock.unlock();
    serverRespCv_.notify_all();
}

void DhtProxyServer::dispatchResponses()
{
    std::unique_lock<std::mutex> lock(serverRespLock_);
    do {
        // wait for a response
        serverRespCv_.wait(lock, [this]{
            return (serverRespQueue_.size() || stopServer_);
        });
        // with the lock
        if (!stopServer_ && serverRespQueue_.size()){
            auto resp_op = std::move(serverRespQueue_.front());
            serverRespQueue_.pop();
            lock.unlock();
            // execute the request
            resp_op();
            lock.lock();
        }
    } while (!stopServer_);
}

request_status DhtProxyServer::options(restinio::request_handle_t request,
                                       restinio::router::route_params_t params)
{
    this->requestCount++;
#ifdef OPENDHT_PROXY_SERVER_IDENTITY
    const auto methods = "OPTIONS, GET, POST, LISTEN, SIGN, ENCRYPT";
#else
    const auto methods = "OPTIONS, GET, POST, LISTEN";
#endif
    auto response = initHttpResponse(request->create_response());
    response.append_header(restinio::http_field::access_control_allow_methods, methods);
    response.append_header(restinio::http_field::access_control_allow_headers, "content-type");
    response.append_header(restinio::http_field::access_control_max_age, "86400");
    return response.done();
}

request_status DhtProxyServer::getNodeInfo(
    restinio::request_handle_t request, restinio::router::route_params_t params)
{
    printf("Connection Id: %lu\n", request->connection_id());
    Json::Value result;
    std::lock_guard<std::mutex> lck(statsMutex);
    if (this->dhtNodeInfo.ipv4.good_nodes == 0 &&
        this->dhtNodeInfo.ipv6.good_nodes == 0){
        this->dhtNodeInfo = this->dhtNode->getNodeInfo();
    }
    result = this->dhtNodeInfo.toJson();
    // [ipv6:ipv4]:port or ipv4:port
    result["public_ip"] = request->remote_endpoint().address().to_string();
    auto output = Json::writeString(this->jsonBuilder, result) + "\n";

    auto response = this->initHttpResponse(request->create_response());
    response.append_body(output);
    return response.done();
}

request_status DhtProxyServer::get(restinio::request_handle_t request,
                                   restinio::router::route_params_t params)
{
    dht::InfoHash infoHash(params["hash"].to_string());
    if (!infoHash)
        infoHash = dht::InfoHash::get(params["hash"].to_string());

    this->scheduleRespDispatch([this, infoHash, request]{

        auto response = std::make_shared<restinio::response_builder_t<response_t>>(
            this->initHttpResponse(request->create_response<response_t>()));
        response->flush();

        this->dhtNode->get(infoHash, [this, response] (const dht::Sp<dht::Value>& value){
            auto output = Json::writeString(this->jsonBuilder, value->toJson()) + "\n";
            response->append_chunk(output);
            response->flush();
            return true;
        },
        [response] (bool /*ok*/){
            response->done();
        });
    });
    return restinio::request_handling_status_t::accepted;
}

request_status DhtProxyServer::put(restinio::request_handle_t request,
                                   restinio::router::route_params_t params)
{
    dht::InfoHash infoHash(params["hash"].to_string());
    if (!infoHash)
        infoHash = dht::InfoHash::get(params["hash"].to_string());

    if (request->body().empty()) {
        auto response = this->initHttpResponse(request->create_response(
            restinio::status_bad_request()));
        response.set_body(this->RESP_MSG_MISSING_PARAMS);
        return response.done();
    }

    std::string err;
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    auto* char_data = reinterpret_cast<const char*>(request->body().data());
    auto reader = std::unique_ptr<Json::CharReader>(rbuilder.newCharReader());

    if (reader->parse(char_data, char_data + request->body().size(), &root, &err)){
        // Build the dht::Value from json, NOTE: {"data": "base64value", ...}
        auto value = std::make_shared<dht::Value>(root);
        bool permanent = root.isMember("permanent");
        std::cout << "Got put " << infoHash << " " << *value <<
                     " " << (permanent ? "permanent" : "") << std::endl;
        /*
        if (permanent){
        }
        else {
        }
        */
        this->scheduleRespDispatch([this, infoHash, request, value, permanent]{
            this->dhtNode->put(infoHash, value, [this, request, value](bool ok){
                if (ok){
                    Json::StreamWriterBuilder wbuilder;
                    wbuilder["commentStyle"] = "None";
                    wbuilder["indentation"] = "";
                    auto output = Json::writeString(this->jsonBuilder, value->toJson()) + "\n";
                    std::cout << output << std::endl;
                    auto response = this->initHttpResponse(request->create_response());
                    response.append_body(output);
                    response.done();
                } else {
                    auto response = this->initHttpResponse(request->create_response(
                        restinio::status_bad_gateway()));
                    response.set_body(this->RESP_MSG_PUT_FAILED);
                    response.done();
                }

            }, dht::time_point::max(), permanent);
        });
    }
    return restinio::request_handling_status_t::accepted;
}

int main()
{
    auto dhtNode = std::make_shared<dht::DhtRunner>();
    dhtNode->run(4444, dht::crypto::generateIdentity(), true);
    dhtNode->bootstrap("bootstrap.jami.net", "4222");

    DhtProxyServer dhtproxy {dhtNode, 8080};
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    };
}

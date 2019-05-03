// Vsevolod Ivanov

#include "proxy_restinio.h"

DhtProxyServer::DhtProxyServer(): node(new dht::DhtRunner)
{
    this->node->run(4444, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    jsonBuilder["commentStyle"] = "None";
    jsonBuilder["indentation"] = "";
    // TODO settings definition: content-type, connection, access-control
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
}

std::unique_ptr<RestRouter> DhtProxyServer::createRestRouter()
{
    auto restRouter = std::make_unique<RestRouter>();

    using namespace std::placeholders;
    restRouter->http_get("/",
                         std::bind(&DhtProxyServer::getNodeInfo, this, _1, _2));
    restRouter->http_get("/:hash",
                         std::bind(&DhtProxyServer::get, this, _1, _2));
    restRouter->http_put("/:hash",
                         std::bind(&DhtProxyServer::put, this, _1, _2));
    return restRouter;
}

int DhtProxyServer::run()
{
    using namespace std::chrono;
    try {
        restinio::run(
            restinio::on_this_thread<RestRouteTraits>()
                .address("0.0.0.0") .port(8080)
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

request_status DhtProxyServer::getNodeInfo(
    restinio::request_handle_t request, restinio::router::route_params_t params)
{
    Json::Value result;
    std::lock_guard<std::mutex> lck(statsMutex);
    if (this->nodeInfo.ipv4.good_nodes == 0 &&
        this->nodeInfo.ipv6.good_nodes == 0){
        nodeInfo = node->getNodeInfo();
    }
    result = this->nodeInfo.toJson();
    // [ipv6:ipv4]:port or ipv4:port
    result["public_ip"] = request->remote_endpoint().address().to_string();
    auto output = Json::writeString(this->jsonBuilder, result) + "\n";

    auto response = request->create_response();
    response.append_body(output);
    return response.done();
}

request_status DhtProxyServer::get(restinio::request_handle_t request,
                                   restinio::router::route_params_t params)
{
    auto response = request->create_response();

    dht::InfoHash infoHash(params["hash"].to_string());
    if (!infoHash)
        infoHash = dht::InfoHash::get(params["hash"].to_string());

    std::mutex done_mutex;
    std::condition_variable done_cv;
    std::unique_lock<std::mutex> done_lock(done_mutex);

    this->node->get(infoHash, [&](const dht::Sp<dht::Value>& value){
        auto output = Json::writeString(jsonBuilder, value->toJson()) + "\n";
        response.append_body(output);
        return true;
    }, [&] (bool /*ok*/){
        std::cout << "done" << std::endl;
        done_cv.notify_all();
    });

    done_cv.wait_for(done_lock, std::chrono::seconds(10));
    return response.done();
}

request_status DhtProxyServer::put(restinio::request_handle_t request,
                                   restinio::router::route_params_t params)
{

    int content_length = request->header().content_length();
    dht::InfoHash infoHash(params["hash"].to_string());
    if (!infoHash)
        infoHash = dht::InfoHash::get(params["hash"].to_string());

    if (request->body().empty()) {
        auto response = request->create_response(restinio::status_bad_request());
        response.set_body(this->RESPONSE_MISSING_PARAMS);
        return response.done();
    }

    std::string err;
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    auto* char_data = reinterpret_cast<const char*>(request->body().data());
    auto reader = std::unique_ptr<Json::CharReader>(rbuilder.newCharReader());

    /*
    if (reader->parse(char_data, char_data + b.size(), &root, &err)) {
        // Build the Value from json
        auto value = std::make_shared<Value>(root);
        bool permanent = root.isMember("permanent");
        std::cout << "Got put " << infoHash << " " << *value <<
                     " " << (permanent ? "permanent" : "") << std::endl;
        if (permanent) {
        }
    }
    */

    auto response = request->create_response();
    response.append_body(std::to_string(content_length));
    return response.done();
}

int main()
{
    DhtProxyServer dhtproxy;
    int error_status = dhtproxy.run();
    return error_status;
}

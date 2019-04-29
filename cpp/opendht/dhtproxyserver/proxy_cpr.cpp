// Vsevolod Ivanov

#include "proxy_cpr.h"

DhtProxyServer::DhtProxyServer() : node(new dht::DhtRunner)
{
    this->node->run(42662, dht::crypto::generateIdentity(), true);
    this->node->bootstrap("bootstrap.jami.net", "4222");

    this->shutdown_mutex.lock();
    this->server = mg_create_server(NULL, evHandler);
}

DhtProxyServer::~DhtProxyServer()
{
    this->node->join();
}

static int hello(struct mg_connection* conn)
{
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;

static int evHandler(struct mg_connection* conn, enum mg_event ev) {
    switch (ev)
    {
        case MG_REQUEST:
            if (Url{conn->uri} == "/")
            {
                return options(conn);
            }
            else if (cpr::Url{conn->uri} == "/hello")
            {
                return hello(conn);
            }
            return MG_FALSE;
        default:
            return MG_FALSE;
    }
}

void DhtProxyServer::run()
{
    std::unique_lock<std::mutex> server_lock(this->server_mutex);
    std::thread(this->mainloop, this->server).detach();
    this->server_cv.wait(server_lock);
}

void DhtProxyServer::mainloop(struct mg_server* server)
{
    {
        std::lock_guard<std::mutex> this->server_lock(this->server_mutex);
        mg_set_option(server, "listening_port", 9080);
        this->server_cv.notify_one();
    }
    do
    {
        mg_poll_server(this->server, 1000);
    }
    while (!this->shutdown_mutex.try_lock());

    this->shutdown_mutex.unlock();
    // TODO move to destructor
    std::lock_guard<std::mutex> server_lock(this->server_mutex);
    mg_destroy_server(&this->server);
    this->server_cv.notify_one();
}

int main(int argc, char *argv[])
{
    DhtProxyServer dhtProxy;
    dhtProxy.run();
}

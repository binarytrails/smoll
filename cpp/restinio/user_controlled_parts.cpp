// Vsevolod Ivanov

#include <chrono>
#include <restinio/all.hpp>

using rest_router = restinio::router::express_router_t<>;
using router_traits = restinio::traits_t<
    restinio::asio_timer_manager_t,
    restinio::single_threaded_ostream_logger_t,
rest_router>;
using request_status = restinio::request_handling_status_t;

class Server
{
    public:
        Server(const int port);
        ~Server();

        template <typename http_resp>
        http_resp init_http_resp(http_resp response);

        request_status get(restinio::request_handle_t request,
                           restinio::router::route_params_t params);

    private:
        std::thread serverThread {};
        std::unique_ptr<rest_router> router();
};

Server::Server(const int port)
{
    this->serverThread = std::thread([this, port](){
        using namespace std::chrono;
        auto maxThreads = std::thread::hardware_concurrency() - 1;
        auto restThreads = maxThreads > 1 ? maxThreads : 1;
        auto settings = restinio::on_thread_pool<router_traits>(restThreads);
        settings.address("0.0.0.0");
        settings.port(port);
        settings.request_handler(this->router());
        settings.read_next_http_message_timelimit(10s);
        settings.write_http_response_timelimit(1s);
        settings.handle_request_timeout(1s);
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

Server::~Server()
{
}

std::unique_ptr<rest_router> Server::router()
{
    using namespace std::placeholders;
    auto router = std::make_unique<rest_router>();
    router->http_get("/", std::bind(&Server::get, this, _1, _2));
    return router;
}

template <typename http_resp>
http_resp Server::init_http_resp(http_resp response)
{
    response.append_header("Server", "RESTinio");
    response.append_header(restinio::http_field::access_control_allow_origin, "*");
    response.connection_keep_alive();
    return response;
}

request_status Server::get(restinio::request_handle_t request,
                         restinio::router::route_params_t params)
{
    printf("connection_id: %lu\n", request->connection_id());

    using output_t = restinio::user_controlled_output_t;
    auto response = this->init_http_resp(request->create_response<output_t>());
    response.flush();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string part1 = "i like";
    response.set_body(part1);
    response.set_content_length(part1.size());
    response.flush();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string part2 = " waffles!";
    response.set_body(part2);
    response.set_content_length(part2.size());
    response.flush();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return response.done();
}

int main()
{
    Server server {8080};
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    };
}

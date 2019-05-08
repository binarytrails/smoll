// Vsevolod Ivanov

#include <httpserver.hpp>

class Resource : public httpserver::http_resource
{
    public:
        const std::shared_ptr<httpserver::http_response> render(
            const httpserver::http_request&
        ){
            auto resp = new httpserver::string_response("hello world!");
            return std::shared_ptr<httpserver::http_response>(resp);
        }
};

int main()
{
    httpserver::webserver server = httpserver::create_webserver(8080);
    Resource resource;
    server.register_resource("/hello", &resource);
    server.start(true);
    return 0;
}

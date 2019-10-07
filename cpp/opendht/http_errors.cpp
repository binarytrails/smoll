// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <fstream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/http.h>

int main(int argc, char * argv[])
{
    std::cout << "./binary <http_code>" << std::endl;
    std::string http_code {argv[1]};

    using namespace dht;
    asio::io_context io_context;
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    auto request = std::make_shared<http::Request>(io_context, "http://httpstat.us/"+http_code, logger);

    restinio::http_request_header_t header;
    header.method(restinio::http_method_get());
    request->set_header(header);
    request->set_target("/"+http_code);
    request->set_header_field(restinio::http_field_t::host, "httpstat.us");
    request->set_header_field(restinio::http_field_t::user_agent, "client");
    request->set_header_field(restinio::http_field_t::accept, "*/*");
    request->set_header_field(restinio::http_field_t::content_type, "application/json");

    std::weak_ptr<http::Request> wreq = request;
    request->add_on_state_change_callback([logger, &io_context, wreq]
                                          (const http::Request::State state, const http::Response response){
        logger->w("state=%i code=%i", state, response.status_code);
        if (state == http::Request::State::RECEIVING){
            auto request = wreq.lock();
            std::cout << "request:\n" << request->to_string() << std::endl;
        }
        else if (state == http::Request::State::DONE){
            std::cout << "response:\n" << response.body << std::endl;
            if (response.status_code != 200)
                logger->e("failed with code=%i", response.status_code);
            else
                logger->w("request done!");
            auto request = wreq.lock();
            request.reset();
            io_context.stop();
        }
    });
    request->send();

    auto work = asio::make_work_guard(io_context);
    io_context.run();

    return 0;
}

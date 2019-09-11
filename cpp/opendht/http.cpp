// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/http.h>

int main(int argc, char * argv[])
{
    if (argc < 2){
        printf("./run <url> <body>\n");
        return 1;
    }
    std::string url = argv[1];
    std::string body;
    if (argv[2])
        body = argv[2];

    using namespace dht;
    asio::io_context io_context;
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    auto request = std::make_shared<http::Request>(io_context, url, logger);

    restinio::http_request_header_t header;
    header.method(restinio::http_method_get());
    request->set_header(header);
    request->set_header_field(restinio::http_field_t::host, url);
    request->set_header_field(restinio::http_field_t::user_agent, "RESTinio client");
    request->set_header_field(restinio::http_field_t::accept, "*/*");
    request->set_header_field(restinio::http_field_t::content_type, "application/json");
    request->set_header_field(restinio::http_field_t::expect, "100-continue");
    if (!body.empty())
        request->set_body(body);

    request->add_on_status_callback([logger](unsigned int status_code){
        logger->d("status: %i", status_code);
    });
    request->add_on_body_callback([logger](const char* at, size_t length){
        try {
            Json::CharReaderBuilder rbuilder;
            auto body = std::string(at, length);
            // one value per body line
            std::string data_line;
            std::stringstream body_stream(body);
            while (std::getline(body_stream, data_line, '\n')){
                std::string err;
                Json::Value json;
                auto* char_data = static_cast<const char*>(&data_line[0]);
                auto reader = std::unique_ptr<Json::CharReader>(rbuilder.newCharReader());
                if (!reader->parse(char_data, char_data + data_line.size(), &json, &err)){
                    logger->e("parsing failed");
                    return;
                }
                std::cout << json << std::endl;
            }
        } catch(const std::exception& e) {
            logger->e("body parsing error: %s", e.what());
        }
    });
    std::weak_ptr<http::Request> wreq = request;
    request->add_on_state_change_callback([logger, &io_context, wreq]
                                          (const http::Request::State state, const http::Response response){
        logger->w("state=%i code=%i", state, response.status_code);
        if (state == http::Request::State::RECEIVING){
            auto request = wreq.lock();
            std::cout << "request:\n" << request->to_string() << std::endl;
        }
        else if (state == http::Request::State::DONE){
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

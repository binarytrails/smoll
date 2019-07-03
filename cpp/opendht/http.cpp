// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/http.h>

int main(int argc, char * argv[])
{
    if (argc < 3){
        printf("./run <host> <service>\n");
        return 1;
    }
    std::string host = argv[1];
    std::string service = argv[2];
    const std::string target = "/";

    // init client context
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    asio::io_context ctx;

    // create http request
    restinio::http_request_header_t header;
    header.request_target(target);
    restinio::http_header_fields_t header_fields;
    header.method(restinio::http_method_get());
    header_fields.append_field(restinio::http_field_t::host, (host + ":" + service).c_str());
    header_fields.append_field(restinio::http_field_t::user_agent, "RESTinio client");
    header_fields.append_field(restinio::http_field_t::accept, "*/*");
    //header_fields.append_field(restinio::http_field_t::content_type, "application/json");

    // setup http_parser & callbacks
    auto parser_s = std::make_shared<http_parser_settings>();
    http_parser_settings_init(parser_s.get());
    parser_s->on_status = []( http_parser * parser, const char* at, size_t length ) -> int {
        printf("on status cb code=%i message=%s\n", parser->status_code, std::string(at, length).c_str());
        return 0;
    };
    parser_s->on_body = []( http_parser * parser, const char * at, size_t length ) -> int {
        printf("on body cb  message={%s} ", std::string(at, length).c_str());
        return 0;
    };
    parser_s->on_message_complete = [](http_parser * parser) -> int {
        printf("on message complete cb\n");
        return 0;
    };
    auto parser = std::make_shared<http_parser>();
    http_parser_init(parser.get(), HTTP_RESPONSE);

    // create client
    http::Client client(ctx, host, service, logger, false/*resolve*/);

    // build request
    auto connection = restinio::http_connection_header_t::close;
    auto request = client.create_request(header, header_fields, connection, ""/*body*/);
    printf(request.c_str());

    // ensure resolving completion
    client.async_resolve(host, service, [&](const asio::error_code &ec){
        // connect
        client.async_connect([&client, request, parser, parser_s]
                             (std::shared_ptr<http::Connection> conn){
            // send the request
            client.async_request(conn, request, parser, parser_s);
        });
    });

    client.async_resolve(host, service, [&](const asio::error_code &ec){
        // connect
        client.async_connect([&client, request, parser, parser_s]
                             (std::shared_ptr<http::Connection> conn){
            // timeout the connection
            client.set_connection_timeout(conn->id(), std::chrono::seconds(0));
        });
    });

    auto work = asio::make_work_guard(ctx);
    ctx.run();

    return 0;
}

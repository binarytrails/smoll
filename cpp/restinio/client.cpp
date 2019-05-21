// Vsevolod Ivanov

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <restinio/all.hpp>
#include <http_parser.h>

template <typename LAMBDA>
void do_with_socket(LAMBDA && lambda, const std::string & addr = "127.0.0.1",
                                      std::uint16_t port = 8080)
{
    restinio::asio_ns::io_context io_context;
    restinio::asio_ns::ip::tcp::socket socket{io_context};

    restinio::asio_ns::ip::tcp::resolver resolver{io_context};
    restinio::asio_ns::ip::tcp::resolver::query
        query{restinio::asio_ns::ip::tcp::v4(), addr, std::to_string(port)};
    restinio::asio_ns::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    restinio::asio_ns::connect(socket, iterator);

    lambda(socket, io_context);
    socket.close();
}

inline void do_request(const std::string & request,
                       const std::string & addr, std::uint16_t port,
                       http_parser &parser, http_parser_settings &settings)
{
    std::string result;
    do_with_socket([&](auto & socket, auto & io_context){
        // write request
        restinio::asio_ns::streambuf b;
        std::ostream req_stream(&b);
        req_stream << request;
        restinio::asio_ns::write(socket, b);
        // read response
        std::array<char, 1024> m_read_buffer;
        socket.async_read_some(restinio::asio_ns::buffer(m_read_buffer), [&](auto ec, size_t length){
            std::vector<char> data;
            data.insert(std::end(data), std::begin(m_read_buffer), std::begin(m_read_buffer) + length);

            auto nparsed = http_parser_execute(&parser, &settings, data.data(), data.size());
            if (HPE_OK != parser.http_errno && HPE_PAUSED != parser.http_errno){
                auto err = HTTP_PARSER_ERRNO(&parser);
                std::cerr << "Couldn't parse the response: " << http_errno_name(err) << std::endl;
            }
        });
        io_context.run();
    }, addr, port);
}

std::string create_http_request(const restinio::http_request_header_t header,
                                const restinio::http_header_fields_t header_fields,
                                const restinio::http_connection_header_t connection,
                                const std::string body)
{
    std::stringstream request;

    request << restinio::method_to_string(header.method()) << " " <<
               header.request_target() << " " <<
               "HTTP/" << header.http_major() << "." << header.http_minor() << "\r\n";
    for (auto header_field: header_fields)
        request << header_field.name() << ": " << header_field.value() << "\r\n";

    std::string conn_str;
    switch (connection)
    {
        case restinio::http_connection_header_t::keep_alive:
            conn_str = "keep-alive";
            break;
        case restinio::http_connection_header_t::close:
            conn_str = "close";
            break;
        case restinio::http_connection_header_t::upgrade:
            throw std::invalid_argument("upgrade");
            break;
    }
    request << "Connection: " << conn_str << "\r\n";
    if (!body.empty()){
        request << "Content-Length: " << body.size() << "\r\n\r\n";
        request << body;
    }
    request << "\r\n\r\n";

    return request.str();
}

int main(const int argc, char* argv[])
{
    if (argc < 5){
        std::cerr << "Insufficient arguments! Needs <method> <addr> <port> <target> <body_if_any>" << std::endl;
        return 1;
    }
    const std::string method_str = argv[1];
    const std::string addr = argv[2];
    const in_port_t port = atoi(argv[3]);
    const std::string target = argv[4];
    const std::string body = argv[5] ? argv[5] : "";

    restinio::http_request_header_t header;
    header.request_target(target);
    restinio::http_method_t method;
    if (method_str == "get")
        method = restinio::http_method_t::http_get;
    else if (method_str == "post")
        method = restinio::http_method_t::http_post;
    else
        throw std::invalid_argument(method_str);
    header.method(method);

    restinio::http_header_fields_t header_fields;
    header_fields.append_field(restinio::http_field_t::host,
                               (addr + ":" + std::to_string(port)).c_str());
    header_fields.append_field(restinio::http_field_t::user_agent, "RESTinio client");
    header_fields.append_field(restinio::http_field_t::accept, "*/*");

    // build request
    auto connection = restinio::http_connection_header_t::keep_alive;
    auto request = create_http_request(header, header_fields, connection, body);
    printf(request.c_str());
    return 1;

    // setup http_parser & callbacks
    http_parser_settings settings; // = restinio::impl::create_parser_settings();
    http_parser_settings_init(&settings);
    settings.on_url = []( http_parser * parser, const char * at, size_t length ) -> int {
        printf("on url cb\n");
        return 0;
    };
    settings.on_header_field = [](http_parser * parser, const char * at, size_t length) -> int {
        printf("on header field cb\n");
        return 0;
    };
    settings.on_header_value = []( http_parser * parser, const char * at, size_t length ) -> int {
        printf("on header value cb\n");
        return 0;
    };
    settings.on_headers_complete = []( http_parser * parser ) -> int {
        printf("on headers complete cb\n");
        return 0;
    };
    settings.on_body = []( http_parser * parser, const char * at, size_t length ) -> int {
        printf("on body cb\n");
        return 0;
    };
    settings.on_message_complete = [](http_parser * parser) -> int {
        printf("on message complete cb\n");
        return 0;
    };
    settings.on_status = []( http_parser * parser, const char * at, size_t length ) -> int {
        printf("on status cb code=%i message=%s\n", parser->status_code, std::string(at, length).c_str());
        return 0;
    };
    http_parser *m_parser = new http_parser();
    http_parser_init(m_parser, HTTP_RESPONSE);

    // send request and give parser for response processing
    do_request(request, addr, port, *m_parser, settings);

    return 0;
}

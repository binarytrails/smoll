// Vsevolod Ivanov

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <restinio/all.hpp> // version >= 0.5.0
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

inline std::string do_request(const std::string & request,
                              const std::string & addr = "127.0.0.1",
                              std::uint16_t port = 8080)
{
    std::string result;
    do_with_socket([&](auto & socket, auto & io_context)
    {
        restinio::asio_ns::streambuf b;
        std::ostream req_stream(&b);
        req_stream << request;
        restinio::asio_ns::write(socket, b);

        std::ostringstream sout;
        restinio::asio_ns::streambuf response_stream;
        restinio::asio_ns::read_until(socket, response_stream, "\r\n\r\n");

        //sout << &response_stream;

        restinio::asio_ns::error_code error;
        while(restinio::asio_ns::read(socket, response_stream,
                                     restinio::asio_ns::transfer_at_least(1),
                                     error))
        {
            sout << &response_stream;
            printf("== %s", sout.str());
        }

        if (!restinio::error_is_eof(error))
            throw std::runtime_error{fmt::format("read error: {}", error)};

        result = sout.str();
    },
    addr, port);

    return result;
}

// Custom HTTP-methods.
constexpr const restinio::http_method_id_t method_listen{
		HTTP_LISTEN, "LISTEN" };
constexpr const restinio::http_method_id_t method_stats{
		HTTP_STATS, "STATS" };
constexpr const restinio::http_method_id_t method_sign{
		HTTP_SIGN, "SIGN" };
constexpr const restinio::http_method_id_t method_encrypt{
		HTTP_ENCRYPT, "ENCRYPT" };

/* Customization for RESTinio.
 * https://github.com/Stiffstream/restinio/issues/26
 */
struct custom_http_methods_t
{
	static constexpr restinio::http_method_id_t
	from_nodejs( int m ) noexcept
	{
		if( m == method_listen.raw_id() )
			return method_listen;
		else if( m == method_stats.raw_id() )
			return method_stats;
		else if( m == method_sign.raw_id() )
			return method_sign;
		else if( m == method_encrypt.raw_id() )
			return method_encrypt;
		else
			return restinio::default_http_methods_t::from_nodejs( m );
	}
};

std::string create_http_request(const restinio::http_request_header_t header,
                                const restinio::http_header_fields_t header_fields,
                                const restinio::http_connection_header_t connection,
                                const std::string body)
{
    std::stringstream request;

    request << header.method().c_str() << " " <<
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
    request << "\r\n";

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
    restinio::http_method_id_t method;
    if (method_str == "get")
        method = restinio::http_method_get();
    else if (method_str == "post")
        method = restinio::http_method_post();
    else if (method_str == "listen")
        method = custom_http_methods_t::from_nodejs(method_listen.raw_id());
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

    // setup http_parser & callbacks
    http_parser_settings settings;
    //http_parser_settings_init(&settings);

    // send request and give parser for response processing
    do_request(request, addr, port);//, *m_parser, settings);

    return 0;
}

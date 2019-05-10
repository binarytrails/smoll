// Vsevolod Ivanov

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <restinio/all.hpp>

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

        sout << &response_stream;

        restinio::asio_ns::error_code error;
        while(restinio::asio_ns::read(socket, response_stream,
                                     restinio::asio_ns::transfer_at_least(1),
                                     error))
            sout << &response_stream;

        if (!restinio::error_is_eof(error))
            throw std::runtime_error{fmt::format("read error: {}", error)};

        result = sout.str();
    },
    addr, port);

    return result;
}

int main(const int argc, char* argv[])
{
    if (argc < 3)
        return 1;

    const std::string addr = argv[1];
    const in_port_t port = atoi(argv[2]);

    std::stringstream request;
    request << "GET / HTTP/1.1\r\n" <<
        "Host: " << addr.c_str() << "\r\n" <<
        "User-Agent: unit-test\r\n" <<
        "Accept: */*\r\n" <<
        "Connection: close\r\n" <<
        "\r\n";
    printf(request.str().c_str());

    std::string response;
    response = do_request(request.str(), addr, port);

    return 1;
}

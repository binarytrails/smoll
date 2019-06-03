// Vsevolod Ivanov
// http://think-async.com/Asio/asio-1.12.2/doc/asio/reference/io_context.html

#include <array>
#include <iostream>
#include <memory>
#include <asio.hpp>

using ResponseCallback = std::function<void(const std::string data)>;

class Connection
{
    public:
        Connection(const uint16_t id, asio::ip::tcp::socket socket):
            id_(id), socket_(std::move(socket))
        {}

        uint16_t id(){
            return id_;
        }

        void start(asio::ip::tcp::resolver::iterator &r_iter){
            asio::connect(socket_, r_iter);
        }

        bool open(){
            return socket_.is_open();
        }

        std::string read(std::error_code& ec){
            std::string response;
            asio::read(socket_, asio::dynamic_buffer(response), ec);
            return response;
        }

        void write(std::string request, std::error_code& ec){
            asio::write(socket_, asio::dynamic_buffer(request), ec);
        }

        void close(){
            socket_.close();
        }

    private:
        uint16_t id_;
        asio::ip::tcp::socket socket_;
};

class Client
{
    public:
        Client(std::string ip, std::uint16_t port): resolver_(ctx_){
            addr_ = asio::ip::address::from_string(ip);
            port_ = port;
        }

        void post_request(std::string request, const ResponseCallback &respcb){
            // post (run when ready) to io_context
            asio::post(ctx_, [this, request, respcb](){
                this->async_request(request, respcb);
                ctx_.run_one();
            });
        }

        asio::io_context& context(){
            return ctx_;
        }

        void stop(){
            ctx_.stop();
        }

    private:

        void async_request(std::string request, const ResponseCallback respcb){
            auto addr_t = addr_.is_v4() ? asio::ip::tcp::v4() : asio::ip::tcp::v6();
            asio::ip::tcp::resolver::query query{addr_t, addr_.to_string(),
                                                 std::to_string(port_)};

            connId_++;
            auto conn = std::make_shared<Connection>(connId_, std::move(asio::ip::tcp::socket{ctx_}));
            printf("[connection:%i] created\n", conn->id());

            resolver_.async_resolve(query, [this, conn, request, respcb](
                    std::error_code ec, asio::ip::tcp::resolver::results_type results
            ){
                if (ec or results.empty()){
                    printf("[connection:%i] error resolving\n", conn->id());
                    conn->close();
                    return;
                }
                for (auto da = results.begin(); da != results.end(); ++da){
                    printf("[connection:%i] resolved host=%s service=%s\n",
                            conn->id(), da->host_name().c_str(), da->service_name().c_str());
                    conn->start(da);
                    break;
                }
                if (!conn->open()){
                    printf("[connection:%i] error closed connection\n", conn->id());
                    return;
                }
                // send request
                printf("[connection:%i] request write\n", conn->id()); 
                conn->write(request, ec);
                if (ec and ec != asio::error::eof){
                    printf("[connection:%i] error: %s\n", conn->id(), ec.message().c_str()); 
                    return;
                }
                // read response
                printf("[connection:%i] response read\n", conn->id());
                auto data = conn->read(ec);
                if (ec and ec != asio::error::eof){
                    printf("[connection:%i] error: %s\n", conn->id(), ec.message().c_str()); 
                    return;
                }
                respcb(data.c_str());
            });
        }

        asio::io_context ctx_;
        std::uint16_t port_;
        asio::ip::address addr_;
        asio::ip::tcp::resolver resolver_;
        uint16_t connId_ = 0;
};

int main(int argc, char* argv[]){
    if (argc != 4){
        std::cerr << "Usage: ./binary <ip> <port> <target>\n";
        return 1;
    }
    std::stringstream req;
    req << "GET " << std::string(argv[3]) << " HTTP/1.1\r\n" <<
           "Host: 127.0.0.1:8080\r\n" <<
           "Accept: */*\r\n" <<
           "Connection: keep-alive\r\n" <<
           "\r\n";
    auto r = req.str();

    Client client(argv[1], std::atoi(argv[2]));
    try {
        client.post_request(req.str(), [](const std::string &resp){
            printf("=========== 1 ==========\n%s\n", resp.c_str());
        });
        client.post_request(req.str(), [](const std::string &resp){
            printf("=========== 2 ==========\n%s\n", resp.c_str());
        });
        // handlers are invoked only by a thread that is currently calling
        client.context().run();
        // stopping the io_context from running out of work
        auto work = asio::make_work_guard(client.context());
        // all operations and handlers be allowed to finish normally
        work.reset();
    }
    catch (std::exception &ex){
        printf("[main] ! exception: %s\n", ex.what());
        return 1;
    }
    printf("[main] finishing\n");
    return 0;
}

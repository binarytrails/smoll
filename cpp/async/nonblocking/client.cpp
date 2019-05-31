// Vsevolod Ivanov

#include <array>
#include <iostream>
#include <memory>
#include <asio.hpp>

class Session
{
    public:
        Session(asio::ip::tcp::socket& socket): socket_(socket){}

        bool ready_read() const {
            return state_ == reading;
        }

        void read(std::error_code& ec){
            std::string response;
            if (asio::read(socket_, asio::dynamic_buffer(response), ec)){
                read_buffer_ = response;
                printf("%s", read_buffer_.c_str());
            }
        }

        bool ready_write() const {
            return state_ == writing;
        }

        void write(std::string request, std::error_code& ec){
            if (asio::write(socket_, asio::dynamic_buffer(request), ec)){
                state_ = write_buffer_.size() > 0 ? writing : reading;
            }
        }

    private:
        asio::ip::tcp::socket& socket_;
        enum { reading, writing } state_ = writing;
        std::string write_buffer_;
        std::string read_buffer_;
};

class Connection: public std::enable_shared_from_this<Connection>
{
    public:
        Connection(asio::ip::tcp::socket socket): socket_(std::move(socket)){}

        void start(asio::ip::tcp::resolver::iterator &r_iter){
            asio::connect(socket_, r_iter);
        }

        void request(const std::string req){
            auto self(shared_from_this());

            if (session_.ready_write() && !write_in_progress_){
                write_in_progress_ = true;
                socket_.async_wait(asio::ip::tcp::socket::wait_write,
                                  [this, req, self](std::error_code ec){
                    write_in_progress_ = false;
                    printf("write: %s\n", ec.message().c_str());
                    if (!ec)
                        session_.write(req, ec);
                    response();
                });
            }
        }

        void response(){
            auto self(shared_from_this());
            std::string resp;

            if (session_.ready_read() && !read_in_progress_){
                read_in_progress_ = true;

                socket_.async_wait(asio::ip::tcp::socket::wait_read,
                                  [this, resp, self](std::error_code ec){
                    read_in_progress_ = false;
                    printf("read: %s\n", ec.message().c_str());
                    if (!ec)
                        session_.read(ec);
                });
            }
        }

        bool open(){
            return socket_.is_open();
        }

        void close(){
            socket_.close();
        }

    private:
        asio::ip::tcp::socket socket_;
        Session session_{socket_};
        bool read_in_progress_ = false;
        bool write_in_progress_ = false;
};

class Client
{
    public:
        Client(asio::io_context &io_context, std::string ip, std::uint16_t port):
                resolver_(io_context){
            addr_ = asio::ip::address::from_string(ip);
            port_ = port;
        }

        void send_request(std::string request){
            async_request(request);
        }

    private:

        void async_request(std::string r){
            auto addr_t = addr_.is_v4() ? asio::ip::tcp::v4() : asio::ip::tcp::v6();
            asio::ip::tcp::resolver::query query{addr_t, addr_.to_string(),
                                                 std::to_string(port_)};
            asio::ip::tcp::socket socket{resolver_.get_io_context()};
            auto conn = std::make_shared<Connection>(std::move(socket));
            connections_.push_back(conn);

            resolver_.async_resolve(query,[this, conn, r](std::error_code ec,
                                           asio::ip::tcp::resolver::results_type results){
                // ready for write
                if (!ec and !results.empty()){
                    auto da = results.begin();
                    std::cout << "resolved " << "host=" << da->host_name() <<
                                 " service=" << da->service_name() << std::endl;
                    conn->start(da);

                    if (conn->open()){
                        printf("sending:\n%s\n", r.c_str());
                        conn->request(r);
                    }
                    else
                        printf("error: connection closed\n");
                }
                else {
                    printf("resolving error: closing connection\n");
                    conn->close();
                }
            });
        }

        asio::ip::address addr_;
        std::uint16_t port_;
        asio::ip::tcp::resolver resolver_;
        std::vector<std::shared_ptr<Connection>> connections_;
};

int main(int argc, char* argv[]){
    try {
        if (argc != 3){
            std::cerr << "Usage: ./binary <ip> <port>\n";
            return 1;
        }
        std::stringstream req;
        req << "GET / HTTP/1.1\r\n" <<
               "Host: 127.0.0.1:8080\r\n" <<
               "Accept: */*\r\n" <<
               "Connection: keep-alive\r\n" <<
               "\r\n";

        asio::io_context io_context;
        Client client(io_context, argv[1], std::atoi(argv[2]));
        client.send_request(req.str());
        client.send_request(req.str());
        client.send_request(req.str());
        io_context.run();
        printf("finishing..\n");
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

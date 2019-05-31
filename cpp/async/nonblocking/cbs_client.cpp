// Vsevolod Ivanov

#include <array>
#include <iostream>
#include <memory>
#include <asio.hpp>

using ResponseCallback = std::function<void(const std::string data)>;

class Session
{
    public:
        Session(asio::ip::tcp::socket& socket): socket_(socket){}

        bool ready_read() const {
            return state_ == reading;
        }

        std::string read(std::error_code& ec){
            std::string response;
            asio::read(socket_, asio::dynamic_buffer(response), ec);
            return response;
        }

        bool ready_write() const {
            return state_ == writing;
        }

        void write(std::string request, std::error_code& ec){
            if (asio::write(socket_, asio::dynamic_buffer(request), ec)){
                state_ = request.size() > 0 ? writing : reading;
            }
        }

    private:
        asio::ip::tcp::socket& socket_;
        enum { reading, writing } state_ = writing;
};

class Connection: public std::enable_shared_from_this<Connection>
{
    public:
        Connection(const uint16_t id, asio::ip::tcp::socket socket):
            id_(id), socket_(std::move(socket)), session_(socket_)
        {}

        void start(asio::ip::tcp::resolver::iterator &r_iter){
            asio::connect(socket_, r_iter);
        }

        void request(const std::string req, const ResponseCallback &respcb){
            auto self(shared_from_this());

            if (session_.ready_write() && !write_in_progress_){
                write_in_progress_ = true;
                socket_.async_wait(asio::ip::tcp::socket::wait_write,
                                  [this, req, &respcb, self](std::error_code ec){
                    write_in_progress_ = false;
                    printf("[connection:%i] request write %s\n", id(), ec.message().c_str());
                    if (!ec)
                        session_.write(req, ec);
                    // start reading response after thre response is written
                    response(respcb);
                });
            }
        }

        void response(const ResponseCallback &respcb){
            auto self(shared_from_this());
            std::string resp;

            if (session_.ready_read() && !read_in_progress_){
                read_in_progress_ = true;

                socket_.async_wait(asio::ip::tcp::socket::wait_read,
                                  [this, resp, &respcb, self](std::error_code ec){
                    read_in_progress_ = false;
                    printf("[connection:%i] response read %s\n", id(), ec.message().c_str());
                    if (!ec){
                        auto data = session_.read(ec);
                        if (respcb)
                            respcb(data);
                    }
                });
            }
        }

        bool open(){
            return socket_.is_open();
        }

        uint16_t id(){
            return id_;
        }

        void close(){
            socket_.close();
        }

    private:
        uint16_t id_;
        asio::ip::tcp::socket socket_;
        Session session_;
        bool read_in_progress_ = false;
        bool write_in_progress_ = false;
};

class Client
{
    public:
        Client(std::string ip, std::uint16_t port): resolver_(ctx_){
            addr_ = asio::ip::address::from_string(ip);
            port_ = port;
        }

        void send_request(std::string request, const ResponseCallback &respcb){
            async_request(request, respcb);
        }

        void run(){
            ctx_.run();
        }

    private:

        void async_request(std::string r, const ResponseCallback &respcb){
            auto addr_t = addr_.is_v4() ? asio::ip::tcp::v4() : asio::ip::tcp::v6();
            asio::ip::tcp::resolver::query query{addr_t, addr_.to_string(),
                                                 std::to_string(port_)};

            asio::ip::tcp::socket socket{resolver_.get_io_context()};
            auto conn = std::make_shared<Connection>(connId_, std::move(socket));
            connId_++;
            printf("[connection:%i] created\n", conn->id());

            resolver_.async_resolve(query,[this, conn, r, &respcb](
                    std::error_code ec,
                    asio::ip::tcp::resolver::results_type results
            ){
                // ready for write
                if (!ec and !results.empty()){
                    // NOTE: could be multiple DAs (ip:port)
                    auto da = results.begin();
                    printf("[connection:%i] resolved host=%s service=%s\n",
                            conn->id(), da->host_name().c_str(), da->service_name().c_str());
                    conn->start(da);

                    if (conn->open()){
                        printf("[connection:%i] sending\n%s", conn->id(), r.c_str());
                        conn->request(r, respcb);
                    }
                    else
                        printf("[connection:%i] error closed connection\n", conn->id());
                }
                else {
                    printf("[connection:%i] error resolving\n", conn->id());
                    conn->close();
                }
            });
        }

        asio::io_context ctx_;
        asio::ip::address addr_;
        std::uint16_t port_;
        asio::ip::tcp::resolver resolver_;
        uint16_t connId_ = 0;
};

int main(int argc, char* argv[]){
    try {
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

        client.send_request(req.str(), [](const std::string response){
            printf("\n[callback 1]\n%s", response.c_str());
        });
        client.send_request(req.str(), [](const std::string response){
            printf("\n[callback 2]\n%s", response.c_str());
        });
        // FIXME avoid overwritting a callback and pass to the ctx
        client.run();

        printf("\nfinishing..\n");
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

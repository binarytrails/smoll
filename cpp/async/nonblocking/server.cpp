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
            std::string read_buffer_;
            if (asio::read(socket_, asio::dynamic_buffer(read_buffer_), ec)){
                printf("%s", read_buffer_.c_str());
                write_buffer_ = read_buffer_;
                state_ = writing;
            }
        }

        bool ready_write() const {
            return state_ == writing;
        }

        void write(std::error_code& ec){
            if (asio::write(socket_, asio::dynamic_buffer(write_buffer_), ec)){
                state_ = write_buffer_.size() > 0 ? writing : reading;
            }
        }

    private:
        asio::ip::tcp::socket& socket_;
        enum { reading, writing } state_ = reading;
        std::string write_buffer_;
};

class Connection: public std::enable_shared_from_this<Connection>
{
    public:
        Connection(asio::ip::tcp::socket socket): socket_(std::move(socket)){}

        void start(){
            socket_.non_blocking(true);
            operations();
        }

    private:
        void operations(){
            auto self(shared_from_this());

            if (session_.ready_read() && !read_in_progress_){
                read_in_progress_ = true;
                socket_.async_wait(asio::ip::tcp::socket::wait_read,
                                  [this, self](std::error_code ec){
                    read_in_progress_ = false;
                    if (!ec)
                        session_.read(ec);
                    if (!ec || ec == asio::error::would_block)
                        operations();
                    else
                        socket_.close();
                });
            }

            if (session_.ready_write() && !write_in_progress_){
                write_in_progress_ = true;
                socket_.async_wait(asio::ip::tcp::socket::wait_write,
                                  [this, self](std::error_code ec){
                    write_in_progress_ = false;
                    if (!ec)
                        session_.write(ec);
                    if (!ec || ec == asio::error::would_block)
                        operations();
                    else
                        socket_.close();
                });
            }
        }
        asio::ip::tcp::socket socket_;
        Session session_{socket_};
        bool read_in_progress_ = false;
        bool write_in_progress_ = false;
};

class Server
{
    public:
        Server(asio::io_context& io_context, unsigned short port):
                acceptor_(io_context, {asio::ip::tcp::v4(), port}){
            accept();
        }

    private:
        void accept(){
            acceptor_.async_accept([this](std::error_code ec,
                                          asio::ip::tcp::socket socket){
                if (!ec){
                    std::make_shared<Connection>(std::move(socket))->start();
                }
                accept();
            });
        }
        asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]){
    try {
        if (argc != 2){
            std::cerr << "Usage: ./binary <port>\n";
            return 1;
        }
        asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        io_context.run();
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

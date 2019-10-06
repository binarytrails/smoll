// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <fstream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/http.h>

std::vector<uint8_t> loadFile(const std::string& path)
{
    std::vector<uint8_t> buffer;
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Can't read file: "+path);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size > std::numeric_limits<unsigned>::max())
        throw std::runtime_error("File is too big: "+path);
    buffer.resize(size);
    file.seekg(0, std::ios::beg);
    if (!file.read((char*)buffer.data(), size))
        throw std::runtime_error("Can't load file: "+path);
    return buffer;
}

int main(int argc, char * argv[])
{

    if (argc < 4){
        printf("./binary client.key client.crt ca.crt\n");
        return 1;
    }
    using namespace dht;
    asio::io_context io_context;
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    auto request = std::make_shared<http::Request>(io_context, "https://127.0.0.1:8080", logger);

    restinio::http_request_header_t header;
    header.method(restinio::http_method_get());
    request->set_header(header);
    request->set_target("/");
    request->set_header_field(restinio::http_field_t::host, "127.0.0.1:8080");
    request->set_header_field(restinio::http_field_t::user_agent, "RESTinio client");
    request->set_header_field(restinio::http_field_t::accept, "*/*");
    request->set_header_field(restinio::http_field_t::content_type, "application/json");

    dht::crypto::Identity identity{};
    identity.first = std::make_shared<dht::crypto::PrivateKey>(loadFile(argv[1]), "xxxx");
    identity.second = std::make_shared<dht::crypto::Certificate>(loadFile(argv[2]));
    request->set_identity(identity);
    auto blobKey = identity.first->serialize("");
    std::cout << std::string(blobKey.begin(), blobKey.end()) << std::endl;
    std::cout << identity.second->toString(true/*chain*/) << std::endl;

    auto ca_ptr = std::make_shared<dht::crypto::Certificate>(loadFile(argv[3]));
    request->set_certificate_authority(ca_ptr);

    request->add_on_status_callback([logger](unsigned int status_code){
        logger->d("status: %i", status_code);
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
            std::cout << "response:\n" << response.body.c_str() << std::endl;
            if (response.status_code != 200)
                logger->e("failed with code=%i", response.status_code);
            else
                logger->w("request done!");
            auto request = wreq.lock();
            request.reset();
            io_context.stop();
        }
    });
    //std::cout << "press any key to send request..." << std::endl; std::cin.get();
    request->send();

    auto work = asio::make_work_guard(io_context);
    io_context.run();

    return 0;
}

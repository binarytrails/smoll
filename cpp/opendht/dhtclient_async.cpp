// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/dht_proxy_client.h>

int main(int argc, char * argv[])
{
    auto hash = dht::InfoHash::get(argv[1]);
    dht::DhtProxyClient client {[](){}, "127.0.0.1:8080", "client_id"};

    auto &io_context = client.httpIOContext();

    dht::Value::Filter filter;
    client.get(hash, [](const dht::Sp<dht::Value>& value){
        printf("[get::cb] value=%s\n", value->toString().c_str());
        return true;
    },[&](bool ok){
        printf("[get::donecb] ok=%i\n", ok);
    }, std::move(filter));

    io_context.run();
    return 0;
}

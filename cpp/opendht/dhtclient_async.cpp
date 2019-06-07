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
        printf("[get1::cb] value=%s\n", value->toString().c_str());
        return true;
    },[](bool ok){
        printf("[get1::donecb] ok=%i\n", ok);
    }, std::move(filter));

    dht::Value value {"slenderman"};
    client.put(hash, std::move(value), [&](bool ok){
        printf("[put1::donecb] ok=%i\n", ok);
    });

    client.listen(hash, [&](const std::vector<dht::Sp<dht::Value>>& values,
                            bool expired){
        printf("[listen1::cb] values = ");
        for (const auto &value: values)
            std::cout << value->toString() << " ";
        std::cout << std::endl;
        return true;
    }, std::move(filter));

    client.listen(hash, [&](const std::vector<dht::Sp<dht::Value>>& values,
                            bool expired){
        printf("[listen2::cb] values = ");
        for (const auto &value: values)
            std::cout << value->toString() << " ";
        std::cout << std::endl;
        return true;
    }, std::move(filter));

    dht::Value value2 {"spidergurl"};
    client.put(hash, std::move(value2), [&](bool ok){
        printf("[put2::donecb] ok=%i\n", ok);
    });

    while (true)
        io_context.run();

    return 0;
}

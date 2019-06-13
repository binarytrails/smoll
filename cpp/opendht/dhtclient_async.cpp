// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/dht_proxy_client.h>

int main(int argc, char * argv[])
{
    auto hash = dht::InfoHash::get(argv[1]);

    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();

    dht::DhtProxyClient client1([](){}, "127.0.0.1:8080", "client01", logger);
    dht::DhtProxyClient client2([](){}, "127.0.0.1:8080", "client02", logger);

    // send two posts
    dht::Value value {"slenderman"};
    client1.put(hash, std::move(value), [&](bool ok){
        logger->d("[put1::donecb] ok=%i", ok);
    });

    dht::Value value2 {"spidergurl"};
    client1.put(hash, std::move(value2), [&](bool ok){
        logger->d("[put2::donecb] ok=%i", ok);
    });

    // schedule one get
    client1.get(hash, [&](const dht::Sp<dht::Value>& value){
        logger->d("[get1::cb] value=%s", value->toString().c_str());
        return true;
    },[&](bool ok){
        logger->d("[get1::donecb] ok=%i", ok);
    });

    // start a listener NOTE: blocking operation
    client2.listen(hash, [&](const std::vector<dht::Sp<dht::Value>>& values,
                            bool expired){
        std::stringstream ss; ss << "[listen::cb] values = ";
        for (const auto &value: values)
            ss << value->toString() << " ";
        logger->d(ss.str().c_str());
        return true;
    });

    auto t = std::thread([&client2](){
        client2.io_context().run();
    });
    client1.io_context().run();

    return 0;
}

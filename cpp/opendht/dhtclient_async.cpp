// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/dht_proxy_client.h>

int main(int argc, char * argv[])
{
    if (argc < 4){
        printf("./run <hash-key> <proxy-addr> <client-key>\n");
        return 1;
    }

    auto hash = dht::InfoHash::get(argv[1]);
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    dht::DhtProxyClient client([](){}, argv[2], argv[3], logger);

    // start a listener
    auto ltoken = client.listen(hash, [&](
        const std::vector<dht::Sp<dht::Value>>& values, bool expired)
    {
        std::stringstream ss; ss << "[listen::cb] values = ";
        for (const auto &value: values)
            ss << value->toString() << " ";
        logger->d(ss.str().c_str());
        return true;
    });

    // send two posts
    dht::Value value {"slenderman"};
    client.put(hash, std::move(value), [&](bool ok){
        logger->d("[put1::donecb] ok=%i", ok);
    });

    dht::Value value2 {"spidergurl"};
    client.put(hash, std::move(value2), [&](bool ok){
        logger->d("[put2::donecb] ok=%i", ok);
    });

    // schedule one get
    client.get(hash, [&](const dht::Sp<dht::Value>& value){
        logger->d("[get1::cb] value=%s", value->toString().c_str());
        return true;
    },[&](bool ok){
        logger->d("[get1::donecb] ok=%i", ok);
    });

    // call periodic executing callbacks through dhtnode
    std::this_thread::sleep_for(std::chrono::seconds(2));
    client.periodic(nullptr, 0, nullptr, 0);

    // restart all listeners
    std::this_thread::sleep_for(std::chrono::seconds(20));
    client.connectivityChanged(); // restarts listeners
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return 0;
}

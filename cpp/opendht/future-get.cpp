#include <iostream>
#include <opendht.h>
#include <opendht/dht_proxy_client.h>

int main(int argc, char * argv[])
{
    if (argc < 3){
        printf("./run <key> <addr>\n");
        return 1;
    }

    auto hash = dht::InfoHash::get(argv[1]);
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    dht::DhtProxyClient client([](){}, argv[2], "client01", logger);

    // get wrapped with future
    auto p = std::make_shared<std::promise<std::vector<std::shared_ptr< dht::Value >>>>();
    auto values = std::make_shared<std::vector<std::shared_ptr< dht::Value >>>();
    client.get(hash, [=](const std::vector<std::shared_ptr<dht::Value>>& vlist) {
        printf("=============== in future get =================\n");
        values->insert(values->end(), vlist.begin(), vlist.end());
        return true;
    }, [=](bool ok) {
        printf("=============== in future done (%s) =================\n",
                ok ? "true": "false");
        p->set_value(std::move(*values));
    });
    p->get_future();

    // get with a pure callback
    client.get(hash, [&](const std::vector<std::shared_ptr<dht::Value>>& values){
        std::stringstream ss; ss << "[get::cb] values = ";
        for (const auto &value: values)
            ss << value->toString() << " ";
        logger->d(ss.str().c_str());
        return true;
    },[&](bool ok){
        logger->d("[get1::donecb] ok=%i", ok);
    });

    while (true){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        client.periodic(nullptr, 0, nullptr, 0);
    }
    return 0;
}

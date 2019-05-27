// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/dht_proxy_client.h>
#include <condition_variable>

int main(int argc, char * argv[])
{
    auto hash = dht::InfoHash::get(argv[1]);

    dht::DhtProxyClient client {[](){}, "127.0.0.1:8080", "client_id"};
    std::condition_variable doneCv;
    std::mutex doneMutex;
    std::unique_lock<std::mutex> lock(doneMutex);

    bool donePut = false;
    dht::Value value {"slenderman"};
    client.put(hash, std::move(value), [&](bool ok){
        printf("put::donecb=%i\n", ok);
        donePut = true;
        doneCv.notify_one();
    });
    doneCv.wait_for(lock, std::chrono::seconds(10), [&donePut]{return donePut;});

    bool doneGet = false;
    dht::Value::Filter filter;
    client.get(hash, [](const dht::Sp<dht::Value>& value){
        printf("get::cb=%s\n", value->toString().c_str());
        return true;
    },[&](bool ok){
        printf("get::donecb=%i\n", ok);
        doneGet = true;
        doneCv.notify_one();
    }, std::move(filter));
    doneCv.wait_for(lock, std::chrono::seconds(10), [&doneGet]{return doneGet;});
    return 0;
}

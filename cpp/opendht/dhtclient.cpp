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
    bool done = false;

    client.get(hash, [](const dht::Sp<dht::Value>& value){
        printf("getcb=%s\n", value->toString().c_str());
        return true;
    },[&](bool ok){
        printf("donecb=%i\n", ok);
        done = true;
        doneCv.notify_all();
    });

    std::mutex doneMutex;
    std::unique_lock<std::mutex> lock(doneMutex);
    doneCv.wait_for(lock, std::chrono::seconds(10), [&done]{return done;});

    return 0;
}

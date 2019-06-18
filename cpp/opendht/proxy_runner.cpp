/**
 * Vsevolod Ivanov
 *
 * Used for proxy benchmark without a dht instance
 */
#include <string>
#include <iostream>
#include <opendht.h>
#include <opendht/log.h>
#include <opendht/dhtrunner.h>
#include <opendht/dht_proxy_server.h>

int main(int argc, char * argv[])
{
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();

    auto node = std::make_shared<dht::DhtRunner>();
    node->setPushNotificationToken("devicekey");
    node->run(8888 /*non-working to disable*/, /*identity*/{}, /*threaded*/true);

    dht::DhtProxyServer server(node /*dht*/, 8080, "127.0.0.1:8090" /*pushserver*/);
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}

/*
 * Vsevolod Ivanov
 *
 * ./run <key> <value1> <value2> ...
 */

#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>

#include <opendht.h>

int main(int argc, char * argv[])
{
    dht::DhtRunner node;
    auto node_id = dht::InfoHash::get(argv[1]);

    node.run(42222, dht::crypto::generateIdentity(), true);
    node.bootstrap("bootstrap.jami.net", "4222");

    dht::Value node_value;
    std::condition_variable done_cv;
    std::mutex done_mutex;
    bool node_action = false;

    for (int8_t i = 2; i < argc; i++){
        dht::Value node_value = {argv[i]};
        std::cout << "node.put(" << argv[i] << ")" << std::endl;
        node.put(node_id, std::move(node_value), [&](bool ok){
            node_action = true;
            done_cv.notify_all();
        });
        std::unique_lock<std::mutex> done_lock(done_mutex);
        done_cv.wait_for(done_lock, std::chrono::seconds(10), [&]{
            return node_action;
        });
    }
    auto values = node.get(node_id).get();

    dht::Blob blob;
    std::string data;
    for (std::vector<std::shared_ptr<dht::Value>>::iterator it = values.begin();
         it != values.end(); ++it
    ){
        blob = (*it)->data;
        data = std::string(blob.begin(), blob.end());
        std::cout << data.c_str() << std::endl;
    }

    return 0;
}

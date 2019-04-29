// Vsevolod Ivanov

#include <cpr/cpr.h>
#include <mongoose.h>

#include <opendht.h>
#include <string>
#include <chrono>
#include <condition_variable>
//#include <json/json.h>

class DhtProxyServer
{
    public:
        DhtProxyServer();
        ~DhtProxyServer();

        void run();
        void mainloop(struct mg_server* server)
/*
        void get(const & request,
                       response);
        void put(const & request,
                       response);
*/
    private:
        std::shared_ptr<dht::DhtRunner> node;
        struct mg_server* server;
        std::mutex shutdown_mutex;
        std::mutex server_mutex;
        std::condition_variable server_cv;
};

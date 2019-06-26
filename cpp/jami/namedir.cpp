// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <thread>

#include "dring/dring.h"
#include "dring/callmanager_interface.h"
#include "dring/configurationmanager_interface.h"
#include "dring/presencemanager_interface.h"

// jamidht/namedirectory.h
enum class Response : int { found = 0, invalidResponse, notFound, error };
enum class RegistrationResponse : int { success = 0,
                                        invalidName,
                                        alreadyTaken,
                                        error,
                                        incompleteRequest,
                                        signatureVerificationFailed
                                    };

int main(int argc, char * argv[])
{
    if (argc < 2){
        printf("./run <lookup-name>\n");
        return 1;
    }

    const std::string ns = "ns.jami.net";

    using std::bind;
    using DRing::exportable_callback;
    using DRing::ConfigurationSignal;

    using SharedCallback = std::shared_ptr<DRing::CallbackWrapperBase>;

    auto registeredNameFoundCb = exportable_callback<ConfigurationSignal::RegisteredNameFound>([&]
        (const std::string& account_id, int state, const std::string& address, const std::string& name){
            printf("\n=====================================================\n");
            printf("account_id: %s state: %i name: %s\naddress: %s\n",
                   account_id.c_str(), state, name.c_str(), address.c_str()
            );
            printf("=====================================================\n");
        });

    const std::map<std::string, SharedCallback> configEvHandlers = {
        exportable_callback<ConfigurationSignal::IncomingAccountMessage>([]
            (const std::string& accountID, const std::string& from, const std::map<std::string, std::string>& payloads){
            }),
            registeredNameFoundCb,
    };

    if (!DRing::init(DRing::InitFlag(DRing::DRING_FLAG_DEBUG | DRing::DRING_FLAG_CONSOLE_LOG)))
        return -1;

    registerSignalHandlers(configEvHandlers);

    if (!DRing::start())
        return -1;

    DRing::lookupName(""/*account*/, ns, argv[1]);

    while (true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

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
    if (argc < 4){
        printf("./run <address> <account> <password> <username>\n");
        return 1;
    }
    auto address = argv[1];
    auto account = argv[2];
    auto password = argv[3];
    auto username = argv[4];

    const std::string ns = "ns.jami.net";

    using std::bind;
    using DRing::exportable_callback;
    using DRing::ConfigurationSignal;

    using SharedCallback = std::shared_ptr<DRing::CallbackWrapperBase>;

    // registerName
    auto nameRegistrationEndedCb = exportable_callback<ConfigurationSignal::NameRegistrationEnded>([&]
        (const std::string& account_id, int state, const std::string& name){
            printf("\n============== name register ======================\n");
            printf("account_id: %s state: %i name: %s\n",
                   account_id.c_str(), state, name.c_str()
            );
            printf("=====================================================\n");
        });
    // lookupName & loopkupAddress
    auto registeredNameFoundCb = exportable_callback<ConfigurationSignal::RegisteredNameFound>([&]
        (const std::string& account_id, int state, const std::string& address, const std::string& name){
            printf("\n================== lookup ended ===================\n");
            printf("account_id: %s state: %i name: %s\naddress: %s\n",
                   account_id.c_str(), state, name.c_str(), address.c_str()
            );
            printf("=====================================================\n");
        });

    if (!DRing::init(DRing::InitFlag(DRing::DRING_FLAG_DEBUG | DRing::DRING_FLAG_CONSOLE_LOG)))
        return 1;

    const std::map<std::string, SharedCallback> configEvHandlers = {
        nameRegistrationEndedCb,
        registeredNameFoundCb
    };
    registerSignalHandlers(configEvHandlers);

    if (!DRing::start())
        return 1;

    // 0: lookup address
    DRing::lookupAddress(""/*account*/, ns, address);
    // 1: register an existing account
    printf("RegisterName: %s", DRing::registerName(account, password, username) ? "OK" : "FAIL");
    // 2: look it up
    DRing::lookupName(account, ns, username);

    while (true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

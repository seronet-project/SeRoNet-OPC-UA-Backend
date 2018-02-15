/**
 * @author Sebastian Friedl
 * TODO create Constructor with argv,argc
 */
#ifndef SMARTCOMPONENTHPP
#define SMARTCOMPONENTHPP

#include <open62541.h>

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIComponent.h"
#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIStatusCode.h"


namespace SeRoNet {
    namespace Utils {
        class SmartComponent : public Smart::IComponent {

        private:
            UA_ServerConfig *config;
            UA_Server *server;
            UA_Boolean running = true;

        public:
            explicit SmartComponent(const std::string &componentName) : IComponent(componentName) {
                config = UA_ServerConfig_new_default();
                server = UA_Server_new(config);
            };

            Smart::StatusCode run() override {
                UA_Server_run(server, &running);
            }

            Smart::StatusCode blocking(const bool b) override {
                return Smart::StatusCode::SMART_OK;
            }

            UA_Server *getServer() {
                return server;
            }
        };

    }
}

#endif
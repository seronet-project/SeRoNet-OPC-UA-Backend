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
class Component :
    public Smart::IComponent {

 private:
  UA_ServerConfig *config;
  UA_Server *server;
  UA_Boolean running = true;
  UA_UInt16 m_nsIndex1;
  const UA_UInt16 m_nsIndex0 = 0; // @TODO remove nsIndex0

 public:
  explicit Component(const std::string &componentName);

  inline UA_UInt16 getNsIndex1() { return m_nsIndex1; };
  inline UA_UInt16 getNsIndex0() { return m_nsIndex0; };

  Smart::StatusCode run() override;

  Smart::StatusCode blocking(const bool b) override;

  inline UA_Server *getServer() { return server; }
};

}
}

#endif
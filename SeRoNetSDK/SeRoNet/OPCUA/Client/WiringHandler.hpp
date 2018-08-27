//
// Created by ac120371 on 24.08.2018.
//

#ifndef SERONETSDK_WIRINGHANDLER_HPP
#define SERONETSDK_WIRINGHANDLER_HPP

#include <string>
#include <mutex>
#include <functional>
#include "../Server/WiringSlave.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class PATTERNTYPE>
class WiringHandler {
 public:

  WiringHandler(PATTERNTYPE *client ) : m_client(client){}

  virtual ~WiringHandler() = default;

  Smart::StatusCode add(SeRoNet::OPCUA::Server::WiringSlave *wiringSlave,
                        const std::string &portName,
                        std::function<Smart::StatusCode(const std::string &, const std::string &)> connectFunct,
                        std::function<Smart::StatusCode()> disconnectFunct

  ) {
    Smart::StatusCode status;
    m_portName = portName;
    m_wiringSlave = wiringSlave;

    this->remove();

    std::unique_lock<std::mutex> lock(m_mutex);
    status = m_wiringSlave->add(m_portName, connectFunct, disconnectFunct);
    m_portIsManged = (status == Smart::SMART_OK);

    return status;
  }

  Smart::StatusCode remove() {
    Smart::StatusCode status;
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_portIsManged) {
      status = this->m_wiringSlave->remove(m_portName);
      if (status == Smart::SMART_OK) {
        m_portName = "";
        m_portIsManged = false;
      }
    } else {
      status = Smart::SMART_OK;
    }
    return status;
  }

 private:
  PATTERNTYPE *m_client = nullptr;
  std::mutex m_mutex;
  std::string m_portName;
  bool m_portIsManged = false;
  SeRoNet::OPCUA::Server::WiringSlave *m_wiringSlave = nullptr;
};

} // namespace client
} // namespace OPCUA
} // namespace SeRoNet


#endif //SERONETSDK_WIRINGHANDLER_HPP

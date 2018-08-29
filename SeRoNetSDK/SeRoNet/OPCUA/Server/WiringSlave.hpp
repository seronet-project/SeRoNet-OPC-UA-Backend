//
// Created by ac120371 on 23.08.2018.
//

#ifndef SERONETSDK_WIRINGSLAVE_HPP
#define SERONETSDK_WIRINGSLAVE_HPP

#include <functional>
#include <map>
#include "../../DefaultCommObjects/WiringCommObject.hpp"
#include "QueryServerHandler.hpp"
#include "../../Utils/HsUlm/smartProcessingPattern.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// forward declaration
class WiringSlave;

/** @internal
   *  Handler for slave part of wiring pattern.
   *
   *  The wiring handler is called by the internally used query pattern
   *  and connects / disconnects a port with a server.
   */
class WiringQueryHandler : public QueryServerHandler<DefaultCommObjects::WiringCommObject,
                                                     DefaultCommObjects::WiringCommObject> {
 private:
  /// used to access the WiringSlave from the handler
  WiringSlave *wiringSlave = nullptr;

 public:
  /// default constructor
  WiringQueryHandler() = delete;

  /** Constructor.
   *
   * @param slave  <I>WiringSlave</I> needed to access it from the handler
   */
  explicit WiringQueryHandler(WiringSlave *slave) noexcept;

  /// Destructor
  ~WiringQueryHandler() override = default;

  /// handle query method of query handler class
  void handleQuery(const int &id,
                   const DefaultCommObjects::WiringCommObject &request) override; //TODO @xfl int durch QueryID ersetztn
};


/** Slave part of wiring pattern.
 *
 *  The slave part of the wiring pattern is responsible for changing the
 *  port connections within the slave components. It manages the port list
 *  of a component.
 *
 *  The user simply needs exactly one instance of the wiring slave per
 *  component. The WiringSlave provides no user accessible member functions.
 *
 *  Demonstrated in <a href="/drupal/?q=node/51#ninth-example">ninth example</a>
 */

//  The interaction of the master / slave parts of the wiring pattern is
//  based on the query communication pattern using the <I>SmartCommWiring</I>
//  communication object. The service is named <I>wiring</I>.
//
//  The port list contains the port name together with pointers to static
//  internalConnect() and internalDisconnect() methods of the client objects.
//  Those pointers are set when a client is added to the set of ports. They
//  are used to call the connect() / disconnect() methods of the client objects
//  when wiring changes have to be performed.

// TODO @xfl check if handler and threadHandler realy need to be pointer
class WiringSlave {
  friend class WiringQueryHandler;
 private:

  struct PortElement {
    std::string portName;
    bool operator==(const PortElement &rhs) const;
    bool operator!=(const PortElement &rhs) const;
    std::function<Smart::StatusCode(const std::string &, const std::string &)> connectFunction;
    std::function<Smart::StatusCode()> disconnectFunction;
  };

  std::map<std::string, PortElement> ports;

  std::mutex mutex;

  /// management class of the component
  Smart::IComponent *component{};

  ///
  WiringQueryHandler *handler{};

  /// Decorator for WiringHandler
  SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<DefaultCommObjects::WiringCommObject,
                                                 DefaultCommObjects::WiringCommObject> *threadHandler
      {};

  /// query server part
  QueryServer<DefaultCommObjects::WiringCommObject,
              DefaultCommObjects::WiringCommObject> *wiring{};

 public:
  ///
  WiringSlave() = delete;

  /** constructor.
   *
   *  (Currently exception not thrown)
   */
  explicit WiringSlave(Smart::IComponent *component, std::string slaveaddress = "");

  /** destructor.
   *
   */
  virtual ~WiringSlave();

  /**
   *
   */
  DefaultCommObjects::WiringCommObject handleWiring(const DefaultCommObjects::WiringCommObject &request);

  /** @internal Add client to the set of ports.
   *
   *  @param portName  name of the port for the added client
   *  @param tPtr  this pointer of client object (returned to static method)
   *  @param cPtr  address of the internalConnect method of the client object
   *  @param dPtr  address of the internalDisconnect method of the client object
   *
   *  @return status code:
   *    - SMART_OK               : everything is ok and client is added as new port. It can
   *                               now be wired from outside. The connection of the client
   *                               is not changed when being added to the set of ports.
   *    - SMART_PORTALREADYUSED  : client not added since the portname is already in use.
   *    - SMART_ERROR            : something went wrong
   */
  Smart::StatusCode add(const std::string &portName,
                        std::function<Smart::StatusCode(const std::string &, const std::string &)> connectFunction,
                        std::function<Smart::StatusCode()> disconnectFunction);

  /** @internal Remove port.
   *
   *  @param portName  name of the port to be removed
   *
   *  @return status code:
   *    - SMART_OK                : everything is ok and the client is not wireable from outside
   *                                anymore. The connection of the client is not changed when
   *                                being removed from the set of ports.
   *    - SMART_UNKNOWNPORT       : no port with the specified name available.
   *    - SMART_ERROR             : something went wrong.
   */
  Smart::StatusCode remove(const std::string &portName);
};

} // end namespace Server
} // end namespace OPCUA
} // end namespace SeRoNet

#endif //SERONETSDK_WIRINGSLAVE_HPP

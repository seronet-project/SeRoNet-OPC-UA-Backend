//
// Created by ac120371 on 23.08.2018.
//

#ifndef SERONETSDK_WIRINGSLAVE_HPP
#define SERONETSDK_WIRINGSLAVE_HPP

#include "../../CommunicationObjects/DefaultObjects/WiringCommObject.hpp"
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
class WiringHandler : public QueryServerHandler<CommunicationObjects::DefaultObjects::WiringCommObject,
                                                CommunicationObjects::DefaultObjects::WiringCommObject> {
 private:
  /// used to access the WiringSlave from the handler
  WiringSlave *wiringSlave = nullptr;

 public:
  /// default constructor
  WiringHandler() = delete;

  /** Constructor.
   *
   * @param slave  <I>WiringSlave</I> needed to access it from the handler
   */
  explicit WiringHandler(WiringSlave *slave) noexcept;

  /// Destructor
  ~WiringHandler() override = default;

  /// handle query method of query handler class
  void handleQuery(const int &id,
                   const CommunicationObjects::DefaultObjects::WiringCommObject &request) override; //TODO @xfl int durch QueryID ersetztn
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
  friend class WiringHandler;
 private:

  struct PortElement {
    std::string portName;
    void *tPtr;
    bool operator==(const PortElement &rhs) const;
    bool operator!=(const PortElement &rhs) const;
    // voided <this> pointer of client object
    Smart::StatusCode (*connectFunction)(void *,
                                         const std::string &,
                                         const std::string &); // ptr to internalConnect method  //TODO use std:function
    Smart::StatusCode (*disconnectFunction)(void *);                                       // ptr to internalDisconnect method //TODO use std:function
  };

  std::map<std::string, PortElement> ports;

  std::mutex mutex;

  /// management class of the component
  Smart::IComponent *component{};

  ///
  WiringHandler *handler{};

  /// Decorator for WiringHandler
  SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<CommunicationObjects::DefaultObjects::WiringCommObject,
                                                 CommunicationObjects::DefaultObjects::WiringCommObject> *threadHandler
      {};

  /// query server part
  QueryServer<CommunicationObjects::DefaultObjects::WiringCommObject,
              CommunicationObjects::DefaultObjects::WiringCommObject> *wiring{};

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
  CommunicationObjects::DefaultObjects::WiringCommObject handleWiring(const CommunicationObjects::DefaultObjects::WiringCommObject &request);

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
                        void *tPtr,
                        Smart::StatusCode (*cPtr)(void *, const std::string &, const std::string &),
                        Smart::StatusCode (*dPtr)(void *));

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

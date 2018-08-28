///
/// \file Test_Query.cpp
/// \author Christian von Arnim
/// \date 06.08.2018
///

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SeRoNet/Utils/Component.hpp>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>

#include <vector>
#include <SeRoNet/CommunicationObjects/Description/ElementArray.hpp>
#include <SeRoNet/CommunicationObjects/Description/IDescription.hpp>
#include <SeRoNet/CommunicationObjects/Description/ComplexType.hpp>
#include <SeRoNet/CommunicationObjects/Description/IDescription.hpp>
#include <SeRoNet/CommunicationObjects/Description/ElementPrimitives.hpp>

#include <SeRoNet/OPCUA/Client/PushClient.hpp>
#include <SeRoNet/Utils/ActiveQueueInputHandlerDecorator.hpp>
#include <SeRoNet/OPCUA/Server/PushServer.hpp>

class CoordinateObject :
    public Smart::ICommunicationObject {
 private:
  int x = 0;
  int y = 0;
  int z = 0;

 public:
  bool operator==(const CoordinateObject &rhs) const {
    return x == rhs.x &&
        y == rhs.y &&
        z == rhs.z;
  }
  bool operator!=(const CoordinateObject &rhs) const {
    return !(rhs == *this);
  }
  CoordinateObject() = default;

  CoordinateObject(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

  ~CoordinateObject() override = default;

  inline int getX() const { return x; }

  inline int getY() const { return y; }

  inline int getZ() const { return z; }

  inline void setX(const int &x) { this->x = x; }

  inline void setY(const int &y) { this->y = y; }

  inline void setZ(const int &z) { this->z = z; }

  // implementation of the CommunicationObjectBase interface
  std::string getName() const override {
    return "CoordinateObject";
  }

  void to_ostream(std::ostream &os = std::cout) const {
    os << getName() << "(" << getX() << ", " << getY() << ", " << getZ() << ") ";
  }

};

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    CoordinateObject>(CoordinateObject *obj, std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  //  Add Members
  (*ret).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "x",
              std::bind(&CoordinateObject::getX, obj),
              std::bind(&CoordinateObject::setX, obj, std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "y",
              std::bind(&CoordinateObject::getY, obj),
              std::bind(&CoordinateObject::setY, obj, std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "z",
              std::bind(&CoordinateObject::getZ, obj),
              std::bind(&CoordinateObject::setZ, obj, std::placeholders::_1)
          )
      )
  );
  return ret;
}

TEST(Push, PushComplexDataType) {

  /// ------------ Setup Client ------------
  SeRoNet::Utils::Component compClient("Test_Push_PushPrimitive_Client");
  SeRoNet::OPCUA::Client::PushClient<CoordinateObject> pushClient(&compClient);

  /// ------------ Setup Server ------------
  std::string serverCompName("Test_Push_ComplexDataType_Server");
  std::string serviceName("PushPrimitive");
  SeRoNet::Utils::Component compServer(serverCompName);
  SeRoNet::OPCUA::Server::PushServer<CoordinateObject> pushServer(&compServer, serviceName);
  ASSERT_EQ(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServerName(), serverCompName);

  std::thread comp_server_thread(&decltype(compServer)::run, &compServer);
  sleep(1); // startup time
  ASSERT_EQ(SeRoNet::OPCUA::Server::OpcUaServer::instance().isRunning(), true);

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(serverCompName, serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  CoordinateObject clientElement = {4, 5, 6};

  CoordinateObject serverElement = {1, 2, 3};
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  sleep(1); // startup time
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.getUpdateWait(clientElement));
  ASSERT_EQ(serverElement, clientElement);

  compServer.stopRunning();
  comp_server_thread.join();
  EXPECT_TRUE(true);
}
///
/// \file Test_Query.cpp
/// \author Christian von Arnim
/// \date 06.08.2018
///

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//#include <SeRoNet/OPCUA/Server/QueryServer.hpp>
#include <SeRoNet/OPCUA/Client/QueryClient.hpp>
#include <SeRoNet/Utils/Component.hpp>
#include <SeRoNet/Utils/ActiveQueueInputHandlerDecorator.hpp>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>

template<typename T_REQ_RESPONSE>
class MockQueryHandler
    : public SeRoNet::OPCUA::Server::QueryServerHandler<T_REQ_RESPONSE, T_REQ_RESPONSE> {
 public:
  using SeRoNet::OPCUA::Server::QueryServerHandler<T_REQ_RESPONSE, T_REQ_RESPONSE>::QueryServerHandler;
  // Make public availiable
  using SeRoNet::OPCUA::Server::QueryServerHandler<T_REQ_RESPONSE, T_REQ_RESPONSE>::m_server;
  MOCK_METHOD2_T(handleQuery, void(
      const int&, const T_REQ_RESPONSE&));
};

TEST(Query, QueryPrimitive) {
  /// ------------ Setup Server ------------
  std::string serverCompName("Test_Query_QueryPrimitive_Server");
  std::string serviceName("QueryPrimitive");
  SeRoNet::Utils::Component compQServer(serverCompName);
  SeRoNet::OPCUA::Server::QueryServer<int, int> queryServer(&compQServer, serviceName);
  MockQueryHandler<int> qHandler(&queryServer);
  SeRoNet::Utils::ActiveQueueQueryServerHandlerDecoratorImpl<int, int> actHandler(&compQServer, &qHandler);
  actHandler.start();
  std::thread comp_server_thread(&decltype(compQServer)::run, &compQServer);

  EXPECT_CALL(qHandler, handleQuery(::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(
          [&qHandler](const int &id, const int &req) {
            qHandler.m_server->answer(id, req);
          }
      );

  /// ------------ Setup Client ------------
  SeRoNet::Utils::Component compQClient("Test_Query_QueryPrimitive_Client");
  SeRoNet::OPCUA::Client::QueryClient<int, int> queryClient(&compQClient);
  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.connect(serverCompName, serviceName));
  int answer = -1;
  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.query(1, answer));
  EXPECT_EQ(answer, 1);
  compQServer.stopRunning();
  comp_server_thread.join();
  EXPECT_TRUE(true);
}

/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SeRoNet/Utils/Component.hpp>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>

#include <vector>
#include <SeRoNet/CommunicationObjects/Description/ElementArray.hpp>

#include <SeRoNet/OPCUA/Client/QueryClient.hpp>
#include <SeRoNet/OPCUA/Server/QueryServer.hpp>
#include <SeRoNet/Utils/HsUlm/smartProcessingPattern.hpp>

#include "helperClasses/ComponentHelperTest.hpp"
#include "helperClasses/CreateValues.hpp"

template<typename T_REQ_RESPONSE>
class MockQueryHandler
    : public SeRoNet::OPCUA::Server::QueryServerHandler<T_REQ_RESPONSE, T_REQ_RESPONSE> {
 public:
  using IQueryServer = Smart::IQueryServerPattern<T_REQ_RESPONSE,T_REQ_RESPONSE>;
  // Make public availiable
  MOCK_METHOD3_T(handleQuery, void(
      IQueryServer &server, const Smart::QueryIdPtr&, const T_REQ_RESPONSE&));
};

class Query : public ComponentHelperTest {

};

TEST_F(Query, QueryPrimitive) {
  /// ------------ Setup Server ------------
  std::string serviceName("QueryPrimitive");
  auto qHandler = std::make_shared<MockQueryHandler<int>>();
  auto actHandler = std::make_shared<SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<int, int>>(compServer, qHandler);
  SeRoNet::OPCUA::Server::QueryServer<int, int> queryServer(compServer, serviceName, actHandler);

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::QueryClient<int, int> queryClient(compServer);
  startServer();

  /// ------------ Setup Mocks ------------
  EXPECT_CALL(*qHandler, handleQuery(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(
          [&qHandler](Smart::IQueryServerPattern<int,int> &server, const Smart::QueryIdPtr &id, const int &req) {
            server.answer(id, req);
          }
      );

  //EXPECT_EQ(actHandler.start(), 0);

  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.connect(compServer->getName(), serviceName));

  /// ------------ Communicate ------------
  int answer = -1;
  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.query(1, answer));
  EXPECT_EQ(answer, 1);
}

TEST_F(Query, QueryPrimitiveArray) {
  typedef std::vector<int> ioType_t;
  /// ------------ Setup Server ------------
  std::string serviceName("QueryPrimitiveArray");
  auto qHandler = std::make_shared<MockQueryHandler<ioType_t>>();
  auto actHandler = std::make_shared<SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<ioType_t, ioType_t>>(compServer, qHandler);
  SeRoNet::OPCUA::Server::QueryServer<ioType_t, ioType_t> queryServer(compServer, serviceName, actHandler);

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::QueryClient<ioType_t, ioType_t> queryClient(compServer);

  startServer();
  /// ------------ Setup Mocks ------------
  EXPECT_CALL(*qHandler, handleQuery(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(
          [&qHandler](Smart::IQueryServerPattern<ioType_t,ioType_t> &server, const Smart::QueryIdPtr &id, const ioType_t &req) {
            server.answer(id, req);
          }
      );
  //EXPECT_EQ(actHandler.start(), 0);

  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.connect(compServer->getName(), serviceName));

  /// ------------ Communicate ------------
  ioType_t answer = {};
  const ioType_t req = {1, 2, 3};
  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.query(req, answer));
  EXPECT_EQ(answer, req);
}

template<typename T_COMMOBJECT>
class QueryTemplate : public Query {

};

typedef ::testing::Types<bool,
                         std::uint8_t,
                         std::uint16_t,
                         std::uint32_t,
                         std::int8_t,
                         std::int16_t,
                         std::int32_t,
                         float,
                         double> QueryTestTypes;

TYPED_TEST_CASE(QueryTemplate, QueryTestTypes);

TYPED_TEST(QueryTemplate, QueryPrimitiveArrayTypes) {
  typedef std::vector<TypeParam> ioType_t;
  /// ------------ Setup Server ------------
  std::string serviceName("QueryPrimitiveArray");
  auto qHandler = std::make_shared<MockQueryHandler<ioType_t>>();
  auto actHandler = std::make_shared<SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<ioType_t, ioType_t>>(this->compServer, qHandler);
  SeRoNet::OPCUA::Server::QueryServer<ioType_t, ioType_t> queryServer(this->compServer, serviceName, actHandler);

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::QueryClient<ioType_t, ioType_t> queryClient(this->compServer);

  this->startServer();

  /// ------------ Setup Mocks ------------
  EXPECT_CALL(*qHandler, handleQuery(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(
          [&qHandler](Smart::IQueryServerPattern<ioType_t,ioType_t> &server, const Smart::QueryIdPtr &id, const ioType_t &req) {
            server.answer(id, req);
          }
      );
  //EXPECT_EQ(actHandler.start(), 0);

  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.connect(this->compServer->getName(), serviceName));

  /// ------------ Communicate ------------
  ioType_t answer = {};
  const ioType_t req = {
      ValueCreator::createValue<TypeParam>(1),
      ValueCreator::createValue<TypeParam>(2),
      ValueCreator::createValue<TypeParam>(3)
  };
  EXPECT_EQ(Smart::StatusCode::SMART_OK, queryClient.query(req, answer));
  EXPECT_EQ(answer, req);
}

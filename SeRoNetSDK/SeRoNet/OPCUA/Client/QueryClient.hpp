/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "QClientOPCUA.hpp"
#include "NamingServiceOpcUa.hpp"
#include <open62541/client.h>
#include "AsyncAnswerFactoryWithCommObject.hpp"
#include "../../Utils/Component.hpp"

namespace SeRoNet
{
  namespace OPCUA
  {
    namespace Client
    {

      template <class RequestType, class AnswerType>
      class QueryClient : public QClientOPCUA<RequestType, AnswerType>
      {
      public:
        QueryClient(Utils::Component *pComponent);
        ~QueryClient() = default;

        Smart::StatusCode queryRequest(
            const RequestType &request,
            Smart::QueryIdPtr &id) override;

        Smart::StatusCode blocking(const bool blocking) override;

        Smart::StatusCode connect(const std::string &server, const std::string &service) override;
        Smart::StatusCode disconnect() override;

      protected:
        std::shared_ptr<AsyncAnswerFactoryWithCommObject<AnswerType, RequestType>> m_Factory;
        std::shared_ptr<SeRoNet::OPCUA::Client::NamingServiceOpcUa> m_namingService;
        SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;

        open62541Cpp::UA_NodeId m_methodNodeId;
      };

      template <class RequestType, class AnswerType>
      inline QueryClient<RequestType, AnswerType>::QueryClient(
          Utils::Component *pComponent) : QClientOPCUA<RequestType, AnswerType>::QClientOPCUA(pComponent),
                                          m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>(pComponent->getOpcUaServer())),
                                          m_pUaClientWithMutex(nullptr)
      {
      }

      template <class RequestType, class AnswerType>
      inline Smart::StatusCode QueryClient<RequestType, AnswerType>::queryRequest(
          const RequestType &request,
          Smart::QueryIdPtr &id)
      {
        if(!m_Factory)
        {
          return Smart::SMART_DISCONNECTED;
        }
        auto ans = m_Factory->call(request);
        id = std::make_shared<OPCUAQueryID<AnswerType>>(ans);
        return ans != nullptr ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
      }

      template <class RequestType, class AnswerType>
      inline Smart::StatusCode QueryClient<RequestType, AnswerType>::blocking(const bool blocking)
      {
        if (blocking)
        {
          m_Factory->enableBlocking();
        }
        else
        {
          m_Factory->disableBlocking();
        }

        return m_Factory->isBlockingEnabled() == blocking ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
      }

      template <class RequestType, class AnswerType>
      Smart::StatusCode QueryClient<RequestType, AnswerType>::disconnect()
      {
        m_Factory = nullptr;
        m_methodNodeId = UA_NODEID_NULL;
        m_pUaClientWithMutex = nullptr;
        return Smart::SMART_OK;
      }

      template <class RequestType, class AnswerType>
      Smart::StatusCode QueryClient<RequestType, AnswerType>::connect(
          const std::string &server,
          const std::string &service)
      {
        auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
        m_pUaClientWithMutex = retValue.connection;
        m_methodNodeId = retValue.nodeId;
        //FIXME use new Constructor for factory with only one element SeRoNet::OPCUA::Client::UaClientWithMutex_t
        m_Factory = std::make_shared<AsyncAnswerFactoryWithCommObject<AnswerType, RequestType>>(
            m_pUaClientWithMutex->pClient,
            m_pUaClientWithMutex->opcuaMutex,
            m_methodNodeId);
        if(!m_Factory->isAvailable())
        {
          return Smart::SMART_SERVICEUNAVAILABLE;
        }
        return Smart::SMART_OK;
      }

    } // namespace Client
  }   // namespace OPCUA
} // namespace SeRoNet

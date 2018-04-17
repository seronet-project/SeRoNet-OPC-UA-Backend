///
/// \file SendClientOpcUa.hpp
/// \author Christian von Arnim
/// \date 17.04.2018
///
#pragma once

#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartISendClientPattern_T.h"
#include "UaClientWithMutex.hpp"
#include "Converter/CommObjectToUaVariantArray.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"

#include <Open62541Cpp/UA_NodeId.hpp>
#include <open62541/open62541.h>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class DataType>
class SendClientOpcUa :
    public Smart::ISendClientPattern<DataType> {
 public:
  static_assert(
      std::is_base_of<CommunicationObjects::ICommunicationObject, DataType>::value,
      "Template Argument 'DataType' must be a descendant of CommunicationObjects::ICommunicationObject");

  SendClientOpcUa(Smart::IComponent *pComponent,
                  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType pUaClientWithMutex,
                  open62541::UA_NodeId methodNodeId, open62541::UA_NodeId objNodeId);

  Smart::StatusCode connect(const std::string &server, const std::string &service) override {
    return SMART_OK;
  }
  Smart::StatusCode disconnect() override {
    return SMART_OK;
  }
  Smart::StatusCode blocking(const bool blocking) override {
    return SMART_OK;
  }
  Smart::StatusCode send(const DataType &data) override {

    UA_CallRequest callRequ;
    UA_CallRequest_init(&callRequ);
    UA_CallMethodRequest methodRequest;
    UA_CallMethodRequest_init(&methodRequest);
    UA_NodeId_copy(m_objNodeId.NodeId, &(methodRequest.objectId));
    UA_NodeId_copy(m_methodNodeId.NodeId, &(methodRequest.methodId));
    {
      Converter::CommObjectToUaVariantArray convToUaVariantArray(data.getObjectDescription());
      auto varInput = convToUaVariantArray.getValue();
      UA_Array_copy(varInput.Variants,
                    varInput.VariantsSize,
                    (void**) &(methodRequest.inputArguments),
                    &UA_TYPES[UA_TYPES_VARIANT]);

      methodRequest.inputArgumentsSize = convToUaVariantArray.getValue().VariantsSize;
    }
    callRequ.methodsToCall = &methodRequest;
    callRequ.methodsToCallSize = 1;

    UA_StatusCode
        retCall = UA_Client_AsyncService_call(m_pUaClientWithMutex->pClient.get(), callRequ, nullptr, this, nullptr);
    Smart::StatusCode ret;
    switch (retCall) {
      case UA_STATUSCODE_GOOD:ret = Smart::StatusCode::SMART_OK;
        break;
      case UA_STATUSCODE_BADDISCONNECT:ret = Smart::StatusCode::SMART_ERROR_COMMUNICATION;
        break;
      default:ret = Smart::StatusCode::SMART_ERROR_UNKNOWN;
    }

    return ret;
  }
 protected:
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  open62541::UA_NodeId m_methodNodeId;
  open62541::UA_NodeId m_objNodeId;
};

template<class DataType>
SendClientOpcUa<DataType>::SendClientOpcUa(Smart::IComponent *pComponent,
                                           SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType pUaClientWithMutex,
                                           open62541::UA_NodeId methodNodeId,
                                           open62541::UA_NodeId objNodeId)
    : ISendClientPattern(pComponent),
      m_pUaClientWithMutex(pUaClientWithMutex),
      m_methodNodeId(methodNodeId),
      m_objNodeId(objNodeId) {

}

}
}
}

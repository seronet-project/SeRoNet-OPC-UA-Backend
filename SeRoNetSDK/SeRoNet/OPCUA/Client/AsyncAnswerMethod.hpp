/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef ASYNC_ANSWER_METHOD_HPP
#define ASYNC_ANSWER_METHOD_HPP

#include "AsyncAnswer.hpp"

#include <open62541/client_highlevel_async.h>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include <memory>
#include <iostream>
#include <Open62541Cpp/Exceptions/OpcUaErrorException.hpp>
#include <Open62541Cpp/UA_NodeId.hpp>
#include <sstream>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
///template<> is needed for friend definition
template<typename T_RETURN>
static void methodCalledCallback(UA_Client *pClient, void *userdata, UA_UInt32 requestId, UA_CallResponse *response);

template<typename T_RETURN>
class AsyncAnswerMethod : public AsyncAnswer<T_RETURN> {
 public:
  /// Do method call on the server
  AsyncAnswerMethod(IBlocking::instanceStorage_t *, bool blockingEnabled, UA_Client *pClient,
                    open62541Cpp::UA_NodeId objectId, open62541Cpp::UA_NodeId methodId,
                    const open62541Cpp::UA_ArrayOfVariant &inputs);

  virtual void processAnswer(UA_StatusCode result, open62541Cpp::UA_ArrayOfVariant *outputs) = 0;

  ~AsyncAnswerMethod() override;
 protected:
  UA_UInt32 m_requestId = 0;
  UA_Client *m_pClient = nullptr;
 private:
  friend void methodCalledCallback<T_RETURN>(UA_Client *pClient,
                                             void *userdata,
                                             UA_UInt32 requestId,
                                             UA_CallResponse *response);

  /// Callback when an answer has been received
  void methodCalled_callback(UA_Client *pClient, void *userdata,
                             UA_UInt32 requestId, UA_CallResponse *callResponse);
};

/// Callback which gets called when method execution has finished
template<typename T_RETURN>
static void methodCalledCallback(UA_Client *pClient, void *userdata, UA_UInt32 requestId, UA_CallResponse *response) {
  AsyncAnswerMethod<T_RETURN> *pAsyncAnswer = static_cast<AsyncAnswerMethod<T_RETURN> *>(userdata);
  if (pAsyncAnswer != nullptr) {
    pAsyncAnswer->methodCalled_callback(pClient, userdata, requestId, response);
  }

}

template<typename T_RETURN>
inline AsyncAnswerMethod<T_RETURN>::AsyncAnswerMethod(
    IBlocking::instanceStorage_t *instStorage, bool blockingEnabled, UA_Client *pClient,
    open62541Cpp::UA_NodeId objectId, open62541Cpp::UA_NodeId methodId,
    const open62541Cpp::UA_ArrayOfVariant &inputs)
    : m_pClient(pClient), AsyncAnswer<T_RETURN>(instStorage, blockingEnabled) {

  UA_StatusCode retVal =
      UA_Client_call_async(
          pClient,
          *objectId.NodeId,
          *methodId.NodeId,
          inputs.VariantsSize,
          inputs.Variants,
          methodCalledCallback <T_RETURN > ,
          this,
          &m_requestId
      );
  if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);

}

template<typename T_RETURN>
inline AsyncAnswerMethod<T_RETURN>::~AsyncAnswerMethod() {
  if(!this->hasAnswer())
  {
    // Make sure, that the callback is not called anymore.
    UA_Client_modifyAsyncCallback(m_pClient, m_requestId, nullptr, nullptr);
  }
}

////@todo creat an exception hanlding
template<typename T_RETURN>
inline void AsyncAnswerMethod<T_RETURN>::methodCalled_callback(
    UA_Client * /*pClient*/,
    void * /*userdata*/,
    UA_UInt32 /*requestId*/,
    UA_CallResponse *callResponse
) {
  if (!callResponse) {
    assert(callResponse != nullptr);
    return;
  }

  UA_StatusCode resultCode = callResponse->responseHeader.serviceResult;

  std::shared_ptr<open62541Cpp::UA_ArrayOfVariant> outArgs = std::make_shared<::open62541Cpp::UA_ArrayOfVariant>(nullptr, 0);
  ///@todo Check ReturnCode of the output arguments
  if (callResponse->resultsSize > 0) {
    resultCode = callResponse->results[0].statusCode;
    if (callResponse->results[0].outputArgumentsSize > 0) {
      outArgs.reset(new open62541Cpp::UA_ArrayOfVariant(callResponse->results[0].outputArguments,
                                                     callResponse->results[0].outputArgumentsSize));
    } else {
      std::cerr <<"Receive Empty Method Call Response without output arguments." <<std::endl;
    }
    this->processAnswer(resultCode, outArgs.get());
  } else {
    std::cerr <<"Receive Empty Method Call Response." <<std::endl;
  }

}

// this not available as answer might be deleted already
template<>
inline void AsyncAnswerMethod<void *>::methodCalled_callback(
    UA_Client *pClient,
    void *userdata,
    UA_UInt32 requestId,
    UA_CallResponse *callResponse
) {
  if (!callResponse) {
    std::cout << "callResponse is nullptr" << std::endl;
    return;
  }

  UA_StatusCode resultCode = callResponse->responseHeader.serviceResult;
  if (resultCode != UA_STATUSCODE_GOOD)
  {
    std::cout <<"Receive nongood result: " <<UA_StatusCode_name(resultCode) << std::endl;
    return;
  }

  if (callResponse->resultsSize > 0) {
    resultCode = callResponse->results[0].statusCode;
    if (resultCode != UA_STATUSCODE_GOOD)
    {
      std::cout <<"Receive nongood result: " <<UA_StatusCode_name(resultCode) << std::endl;
      return;
    }

    if (callResponse->results[0].outputArgumentsSize != 0) {
      std::cout <<"Receive  Method Call Response with output arguments. No output arguments allowed! Expect 0, got"
        << callResponse->results[0].outputArgumentsSize << std::endl;
      return;
    }
  } else {
    std::cout  << "Receive Empty Method Call Response." << std::endl;
    return;
  }

}

}
}
}

#endif

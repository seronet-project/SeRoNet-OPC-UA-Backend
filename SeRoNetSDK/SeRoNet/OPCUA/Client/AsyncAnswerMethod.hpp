/*
* @author Christian von Arnim
*/

#ifndef ASYNC_ANSWER_METHOD_HPP
#define ASYNC_ANSWER_METHOD_HPP

#include "AsyncAnswer.hpp"

#include <open62541.h>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include <memory>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
///template<> is needed for friend definition
template<typename T_RETURN>
static void methodCalledCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, const void *response);

template<typename T_RETURN>
class AsyncAnswerMethod : public AsyncAnswer<T_RETURN> {
 public:
  /// Do method call on the server
  AsyncAnswerMethod(IBlocking::instanceStorage_t *, bool blockingEnabled, UA_Client *client,
                    UA_NodeId objectId, UA_NodeId methodId,
                    const open62541::UA_ArrayOfVariant &inputs);

  virtual void processAnswer(UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs) = 0;

  virtual ~AsyncAnswerMethod() = default;
 protected:
  UA_UInt32 m_requestId = 0;
 private:
  friend void methodCalledCallback<T_RETURN>(UA_Client *client,
                                             void *userdata,
                                             UA_UInt32 requestId,
                                             const void *response);

  /// Callback when an aswer has been received
  void methodCalled_callback(UA_Client *client, void *userdata,
                             UA_UInt32 requestId, const void *response);
};

/// Callback which gets called when method execution has finished
template<typename T_RETURN>
static void methodCalledCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, const void *response) {
  AsyncAnswerMethod<T_RETURN> *pAsyncAnswer = static_cast<AsyncAnswerMethod<T_RETURN> *>(userdata);
  assert(pAsyncAnswer != nullptr);

  if (pAsyncAnswer != nullptr) {
    pAsyncAnswer->methodCalled_callback(client, userdata, requestId, response);
  }

}

template<typename T_RETURN>
inline AsyncAnswerMethod<T_RETURN>::AsyncAnswerMethod(
    IBlocking::instanceStorage_t *instStorage, bool blockingEnabled, UA_Client *client,
    UA_NodeId objectId, UA_NodeId methodId,
    const open62541::UA_ArrayOfVariant &inputs)
    : AsyncAnswer<T_RETURN>(instStorage, blockingEnabled) {
  UA_CallRequest callRequ;
  UA_CallRequest_init(&callRequ);

  UA_CallMethodRequest methodRequest;
  UA_CallMethodRequest_init(&methodRequest);
  methodRequest.objectId = objectId;
  methodRequest.methodId = methodId;

  methodRequest.inputArguments = inputs.Variants;
  methodRequest.inputArgumentsSize = inputs.VariantsSize;

  callRequ.methodsToCall = &methodRequest;
  callRequ.methodsToCallSize = 1;

  UA_Client_AsyncService_call(client, callRequ, methodCalledCallback < T_RETURN > , this, &m_requestId);

  ///@todo use Open62541Cpp::UA_NodeID as parameter type instead
  UA_NodeId_deleteMembers(&objectId);
  UA_NodeId_deleteMembers(&methodId);
}

template<typename T_RETURN>
inline void AsyncAnswerMethod<T_RETURN>::methodCalled_callback(UA_Client *client,
                                                               void *userdata,
                                                               UA_UInt32 requestId,
                                                               const void *response) {
  const UA_CallResponse *callResponse = static_cast<const UA_CallResponse *>(response);

  if (!callResponse) {
    assert(callResponse != nullptr);
    return;
  }

  UA_StatusCode resultCode = callResponse->responseHeader.serviceResult;

  std::shared_ptr < open62541::UA_ArrayOfVariant > outArgs(nullptr);
  ///@todo Check ReturnCode of the output arguments
  if (callResponse->resultsSize > 0) {
    if (callResponse->results[0].outputArgumentsSize > 0) {
      outArgs.reset(new open62541::UA_ArrayOfVariant(callResponse->results[0].outputArguments,
                                                     callResponse->results[0].outputArgumentsSize));
    }
  }

  this->processAnswer(resultCode, outArgs.get());
}
}
}
}

#endif

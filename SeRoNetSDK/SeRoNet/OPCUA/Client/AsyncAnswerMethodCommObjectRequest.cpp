///
/// \file AsyncAnswerMethodCommObjectRequest.cpp
/// \author Christian von Arnim
/// \date 10.08.2018
///

#include "AsyncAnswerMethodCommObjectRequest.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<>
void AsyncAnswerMethodCommObjectRequest<void *>::processAnswer(
    UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs) {
  std::cout << "syncAnswerMethodCommObjectRequest<void*>::processAnswer" << std::endl;
  if (result != UA_STATUSCODE_GOOD) {
    std::stringstream ss;
    ss << "Receive nongood result: " << UA_StatusCode_name(result);
    this->setError(ss.str());
    return;
  }
}

}
}
}

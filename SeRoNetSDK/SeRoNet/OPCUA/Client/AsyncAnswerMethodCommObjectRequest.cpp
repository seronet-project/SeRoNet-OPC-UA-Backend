/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

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

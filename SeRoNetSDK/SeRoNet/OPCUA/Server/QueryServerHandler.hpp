/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef QUERY_SERVER_HANDLER_HPP
#define QUERY_SERVER_HANDLER_HPP

////#include "SeRoNet/Utils/IObserver.hpp"
#include "QueryServer.hpp"
#include <smartIQueryServerPattern_T.h>
#include <smartICommunicationObject.h>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<typename T_REQUEST, typename T_ANSWER>
class QueryServer;

template<typename T_REQUEST, typename T_ANSWER>
class QueryServerHandler : public Smart::IQueryServerHandler<T_REQUEST, T_ANSWER> {
 public:
  QueryServerHandler() = default;
  ~QueryServerHandler() = default;
};

}
}
}

#endif

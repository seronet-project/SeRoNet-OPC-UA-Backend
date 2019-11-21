/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <memory>
#include <chrono>
#include <sstream>

#include <smartICorrelationId.h>
#include <smartIQueryClientPattern_T.h>
#include "AsyncAnswer.hpp"

#include "../../Exceptions/BlockingDisabledException.hpp"
#include <iostream>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class RequestType, class AnswerType>
class QClientOPCUA;

template<class AnswerType>
class OPCUAQueryID : public Smart::ICorrelationId {
 public:
  typedef std::shared_ptr<SeRoNet::OPCUA::Client::AsyncAnswer<AnswerType>> shpAsyncAnswer_t;
  OPCUAQueryID() = default;
  inline OPCUAQueryID(shpAsyncAnswer_t asyncAnswer) : pAsyncAnswer(asyncAnswer), isValid(true) {}
 private:
  template<class RequestType2, class AnswerType2> friend
  class QClientOPCUA;
  shpAsyncAnswer_t pAsyncAnswer;
  std::atomic_bool isValid = false; /// <@todo replace by function checking for nullptr?!
protected:
  virtual std::string to_string() const override final {
    std::stringstream sstream;
    sstream << pAsyncAnswer;
    return sstream.str();
  }
  virtual bool less_than(const ICorrelationId *other) const override final {
    auto native_other = dynamic_cast<const OPCUAQueryID<AnswerType>*>(other);
    return pAsyncAnswer < native_other->pAsyncAnswer;
  }
  virtual bool equals_to(const ICorrelationId *other) const override final {
    auto native_other = dynamic_cast<const OPCUAQueryID<AnswerType>*>(other);
    return pAsyncAnswer == native_other->pAsyncAnswer;
  }
};

//<alex>: this definition is superseeded by the generic correlation id (see usage below)
//template<class AnswerType>
//using QueryIDtype_template = std::shared_ptr<OPCUAQueryID<AnswerType>>;

template<class RequestType, class AnswerType>
class QClientOPCUA :
    public Smart::IQueryClientPattern<RequestType, AnswerType> {
 public:
  using Smart::IQueryClientPattern<RequestType, AnswerType>::IQueryClientPattern;

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;
  Smart::StatusCode disconnect() override;
  //typedef QueryIDtype_template<AnswerType> QueryIDtype;

  using Smart::IQueryClientPattern<RequestType,AnswerType>::query; // use default implementation
  //Smart::StatusCode query(const RequestType &request, AnswerType &answer) override;
  Smart::StatusCode queryReceive(const Smart::QueryIdPtr id, AnswerType &answer) override;
  Smart::StatusCode queryReceiveWait(const Smart::QueryIdPtr id,
                                     AnswerType &answer,
                                     const Smart::Duration &timeout = Smart::Duration::max()) override;
  ///@todo Remove!? Not needed in an implementation with shared pointern, will allow to read a request multiple
  /// 	times (e.g. from different threads)
  Smart::StatusCode queryDiscard(const Smart::QueryIdPtr id) override;
};

/*
template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::query(const RequestType &request, AnswerType &answer) {
  QueryIDtype_template<AnswerType> qID;
  Smart::StatusCode status = this->queryRequest(request, qID);
  if (status != Smart::StatusCode::SMART_OK) {
    return status;
  }

  return this->queryReceiveWait(qID, answer, std::chrono::microseconds(0));
  //qID will be discarded automatically when the last reference is gone (shared_ptr)
}
*/

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::queryReceive(const Smart::QueryIdPtr id,
                                                                             AnswerType &answer) {
  auto internal_id = std::dynamic_pointer_cast<OPCUAQueryID<AnswerType>>(id);
  if(!internal_id || !internal_id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }

  if (!internal_id->pAsyncAnswer->hasAnswer()) {
    return Smart::StatusCode::SMART_NODATA;
  }
  answer = internal_id->pAsyncAnswer->waitForAnswer();
  this->queryDiscard(id);
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType,
                                      AnswerType>::queryReceiveWait(const Smart::QueryIdPtr id,
                                                                    AnswerType &answer,
                                                                    const Smart::Duration &timeout) {
  auto internal_id = std::dynamic_pointer_cast<OPCUAQueryID<AnswerType>>(id);
  if(!internal_id || !internal_id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }

  try {
    answer = internal_id->pAsyncAnswer->waitForAnswer();
  }
  catch (SeRoNet::Exceptions::BlockingDisabledException &ex) {
    return Smart::StatusCode::SMART_CANCELLED;
  }
  catch (SeRoNet::Exceptions::ResultErrorException &ex) {
    std::cout << "SeRoNet::Exceptions::ResultErrorException: " << ex.what() << std::endl;
    /// \todo correct return code?
    return Smart::StatusCode::SMART_ERROR;
  }

  this->queryDiscard(id);
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::queryDiscard(const Smart::QueryIdPtr id) {
  auto internal_id = std::dynamic_pointer_cast<OPCUAQueryID<AnswerType>>(id);
  if(!internal_id || !internal_id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }
  internal_id->isValid = false;
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::connect(const std::string &server, const std::string &service) {
  ///\hack implement
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::disconnect() {
  ///\hack implement
  return Smart::StatusCode::SMART_OK;
}

}
}
}

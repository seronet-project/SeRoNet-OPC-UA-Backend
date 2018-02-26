///
/// \file QClientOPCUA.hpp
/// \author Christian von Arnim
/// \date 19.02.2018
///
#pragma once

#include <SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIQueryClientPattern_T.h>
#include <SeRoNet/OPCUA/Client/AsyncAnswer.hpp>
#include <memory>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class RequestType, class AnswerType>
class QClientOPCUA;

template<class AnswerType>
class OPCUAQueryID {
 public:
  typedef std::shared_ptr<SeRoNet::OPCUA::Client::AsyncAnswer<AnswerType>> shpAsyncAnswer_t;
  OPCUAQueryID() = default;
  inline OPCUAQueryID(shpAsyncAnswer_t asyncAnswer) : pAsyncAnswer(asyncAnswer), isValid(true) {}
 private:
  template<class RequestType2, class AnswerType2> friend
  class QClientOPCUA;
  shpAsyncAnswer_t pAsyncAnswer;
  std::atomic_bool isValid = false; /// <@todo replace by function checking for nullptr?!
};

template<class AnswerType>
using QueryIDtype_template = std::shared_ptr<OPCUAQueryID<AnswerType>>;

template<class RequestType, class AnswerType>
class QClientOPCUA :
    public Smart::IQueryClientPattern<RequestType, AnswerType, QueryIDtype_template<AnswerType>> {
 public:
  using Smart::IQueryClientPattern<RequestType, AnswerType, QueryIDtype_template<AnswerType>>::IQueryClientPattern;

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;
  Smart::StatusCode disconnect() override;
  typedef QueryIDtype_template<AnswerType> QueryIDtype;

  Smart::StatusCode query(const RequestType &request, AnswerType &answer) override;
  Smart::StatusCode queryReceive(const QueryIDtype &id, AnswerType &answer) override;
  Smart::StatusCode queryReceiveWait(const QueryIDtype &id, AnswerType &answer) override;
  ///@todo Remove!? Not needed in an implementation with shared pointern, will allow to read a request multiple
  /// 	times (e.g. from different threads)
  Smart::StatusCode queryDiscard(const QueryIDtype &id) override;
};

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::query(const RequestType &request, AnswerType &answer) {
  QueryIDtype_template<AnswerType> qID;
  Smart::StatusCode status = this->queryRequest(request, qID);
  if (status != Smart::StatusCode::SMART_OK) {
    return status;
  }

  return this->queryReceiveWait(qID, answer);
  //qID will be discarded automatically when the last reference is gone (shared_ptr)
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::queryReceive(const QueryIDtype &id,
                                                                             AnswerType &answer) {
  if (!id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }

  if (!id->pAsyncAnswer->hasAnswer()) {
    return Smart::StatusCode::SMART_NODATA;
  }
  answer = id->pAsyncAnswer->waitForAnswer();
  this->queryDiscard(id);
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::queryReceiveWait(const QueryIDtype &id,
                                                                                 AnswerType &answer) {
  if (!id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }

  try {
    answer = id->pAsyncAnswer->waitForAnswer();
  }
  catch (SeRoNet::Exceptions::BlockingDisabledException &ex) {
    return Smart::StatusCode::SMART_CANCELLED;
  }

  this->queryDiscard(id);
  return Smart::StatusCode::SMART_OK;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUA<RequestType, AnswerType>::queryDiscard(const QueryIDtype &id) {
  if (!id->isValid) {
    return Smart::StatusCode::SMART_WRONGID;
  }
  id->isValid = false;
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
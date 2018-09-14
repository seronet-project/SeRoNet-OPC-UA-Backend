//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSE_H
#define SERONETSDK_PARAMETERRESPONSE_H

#include <iostream>

// include Message-Data-Struct
#include "CommParameterIDL/ParameterResponseIdl.hpp"

// include enums
#include "EnumParamResponseType.hpp"
#include "../CommunicationObjects/ICommunicationObject.hpp"
#include "smartICommunicationObject.h"

namespace SeRoNet {
namespace DefaultCommObjects {

class CommParameterResponse : public SeRoNet::CommunicationObjects::ICommunicationObject, Smart::ICommunicationObject {
 public:
  CommParameterIDL::CommParameterResponse data;
 private:
  std::string getName() const override;
 public:
  CommParameterResponse() = default;

  explicit CommParameterResponse(const CommParameterIDL::CommParameterResponse &obj) :
      data(obj) {
  }

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t getObjectDescription(std::string name) override;

  virtual ~CommParameterResponse() = default;

  explicit operator CommParameterIDL::CommParameterResponse() const {
    return data;
  }

  static inline std::string identifier(void) {
    return "CommParameter::CommParameterResponse";
  }

  //
  // generated getter and setter methods
  //

  inline ParamResponseType getResponse() const {
    return ParamResponseType(data.response);
  }
  inline void setResponse(const ParamResponseType &response) {
    data.response = response.get_value_idl();
  }

  // helper method to easily implement output stream in derived classes
  void to_ostream(std::ostream &os = std::cout) const;
};

inline std::ostream &operator<<(std::ostream &os, const CommParameterResponse &co) {
  co.to_ostream(os);
  return os;
}

} // end namespace DefaultCommObjects
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERRESPONSE_H


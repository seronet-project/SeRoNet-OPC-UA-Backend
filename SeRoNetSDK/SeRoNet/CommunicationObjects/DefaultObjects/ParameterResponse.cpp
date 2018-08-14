//
// Created by ac120371 on 10.08.2018.
//

#include "ParameterResponse.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {

Description::ComplexType::shp_t CommParameterResponse::getObjectDescription(std::string name) {
  return
      SeRoNet::CommunicationObjects::Description::ComplexType::shp_t();
}
std::string CommParameterResponse::getName() const {
  return CommParameterResponse::identifier();
}
void CommParameterResponse::to_ostream(std::ostream &os) const {

  os << "CommParameterResponse(";
  os << getResponse();
  os << ")";

}

}
}
}
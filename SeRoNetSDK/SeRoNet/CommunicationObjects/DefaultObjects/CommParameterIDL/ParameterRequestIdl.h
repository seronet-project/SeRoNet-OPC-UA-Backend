//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_PARAMETERREQUESTIDL_H
#define SERONETSDK_PARAMETERREQUESTIDL_H

#include <vector>
#include <string>

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {
namespace CommParameterIDL {

typedef std::vector<std::string> NameValue_values_type;

struct NameValue {
  std::string key;
  NameValue_values_type values;
};

typedef std::vector<NameValue> CommParameterRequest_items_type;

struct CommParameterRequest {
  CommParameterRequest_items_type items;
};

} // end namespcae CommParameterIDL
} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERREQUESTIDL_H

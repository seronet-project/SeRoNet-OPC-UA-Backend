//
// Created by ac120371 on 09.08.2018.
//

#pragma once
#include <vector>
#include <string>
#include <functional>

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {
namespace CommParameterIDL {

typedef std::vector<std::string> NameValue_values_type;

struct NameValue {
  std::string key;
  NameValue_values_type values;

 public:
  std::string getKey() const { return this->key; }
  void setKey(std::string key) { this->key = key; }

  NameValue_values_type getValues() const { return this->values; }
  void setValues(NameValue_values_type values) { this->values = values; }

};

typedef std::vector<NameValue> CommParameterRequest_items_type;

struct CommParameterRequest {
  CommParameterRequest_items_type items;
};

} // end namespcae CommParameterIDL
} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end namespace SeRoNet

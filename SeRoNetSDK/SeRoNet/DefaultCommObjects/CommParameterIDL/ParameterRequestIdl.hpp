/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#pragma once
#include <vector>
#include <string>
#include <functional>

namespace SeRoNet {
namespace DefaultCommObjects {
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

  bool operator==(const NameValue &other) const {
    return this->key == other.key
        && this->values == other.values;
  }

};

typedef std::vector<NameValue> CommParameterRequest_items_type;

struct CommParameterRequest {
  bool operator==(const CommParameterRequest &other) const {
    return this->items == other.items;
  }

  CommParameterRequest_items_type items;
};

} // end namespace CommParameterIDL
} // end namespace DefaultCommObjects
} // end namespace SeRoNet

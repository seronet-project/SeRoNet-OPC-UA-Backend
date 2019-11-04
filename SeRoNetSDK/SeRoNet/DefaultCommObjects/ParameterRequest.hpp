/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma  once

#include <string>
#include <list>
#include <iostream>
#include <sstream>

// include core class
#include "CommParameterIDL/ParameterRequestIdl.hpp"

//SeRoNet class
#include "../../Definitions.hpp"

#include "../CommunicationObjects/ICommunicationObject.hpp"
#include <smartICommunicationObject.h>

namespace SeRoNet {
namespace DefaultCommObjects {

class SERONETSDK_EXPORT CommParameterRequest : public CommunicationObjects::ICommunicationObject, public Smart::ICommunicationObject {
 public:
  CommParameterIDL::CommParameterRequest data;

  CommParameterRequest() = default;

  explicit CommParameterRequest(const CommParameterIDL::CommParameterRequest &obj) :
      data(obj) {
  }
  std::string getName() const override;
  virtual ~CommParameterRequest() = default;

  explicit operator CommParameterIDL::CommParameterRequest() const {
    return data;
  }

  static inline std::string identifier(void) {
    return "CommParameter::CommParameterRequest";
  }

  //
  // add your customized interface here
  //
  int remove(const std::string &key);
  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t getObjectDescription(std::string name) override;

  // getTag returns the string value for the top level tag
  std::string getTag() const;

  // setTag writes the string value for the top level tag
  void setTag(const std::string &value);

  /**
   * Generic setter method (expects one of primitive data types for the value)
   */
  template<class Value>
  int set(const std::string &key, const Value &value);

  /*
   * Data type specific setter methods
   */
  inline int setString(const std::string &key, const std::string &value) {
    return set(key, value);
  }
  inline int setInteger(const std::string &key, const int &value) {
    return set(key, value);
  }
  inline int setDouble(const std::string &key, const double &value) {
    return set(key, value);
  }
  inline int setBoolean(const std::string &key, const bool &value) {
    return set(key, value);
  }

  /**
   * Generic getter method (expects one of primitive data types for the value)
   */
  template<class Value>
  int get(const std::string &key, Value &value) const;

  template<class Value>
  int getList(const std::string &key, std::list<Value> &value) const;

  /*
   * Data type specific getter methods (value by reference)
   */

  // string getter can not be inline due to template specialization (see th file)
  int getString(const std::string &key, std::string &value) const;

  template<class Integer>
  inline int getInteger(const std::string &key, Integer &value) const {
    return get(key, value);
  }
  template<class Double>
  inline int getDouble(const std::string &key, Double &value) const {
    return get(key, value);
  }
  inline int getBoolean(const std::string &key, bool &value) const {
    return get(key, value);
  }

  int getStringList(const std::string &key, std::list<std::string> &value) const;

  template<class Integer>
  inline int getIntegerList(const std::string &key, std::list<Integer> &value) const {
    return getList(key, value);
  }
  template<class Double>
  inline int getDoubleList(const std::string &key, std::list<Double> &value) const {
    return getList(key, value);
  }
  inline int getBooleanList(const std::string &key, std::list<bool> &value) const {
    return getList(key, value);
  }

  /*
   * Data type specific getter methods (value by return)
   */

  // string getter can not be inline due to template specialization (see th file)
  std::string getString(const std::string &key) const;

  inline int getInteger(const std::string &key) const {
    int result;
    this->get(key, result);
    return result;
  }
  double getDouble(const std::string &key) const {
    double result;
    this->get(key, result);
    return result;
  }
  bool getBoolean(const std::string &key) const {
    bool result;
    this->get(key, result);
    return result;
  }

  std::list<std::string> getStringList(const std::string &key) const;

  inline std::list<int> getIntegerList(const std::string &key) const {
    std::list<int> result;
    this->getList(key, result);
    return result;
  }
  std::list<double> getDoubleList(const std::string &key) const {
    std::list<double> result;
    this->getList(key, result);
    return result;
  }
  std::list<bool> getBooleanList(const std::string &key) const {
    std::list<bool> result;
    this->getList(key, result);
    return result;
  }

  // helper method to easily implement output stream in derived classes
  void to_ostream(std::ostream &os = std::cout) const;

};

inline std::ostream &operator<<(std::ostream &os, const CommParameterRequest &co)
{
  co.to_ostream(os);
  return os;
}


template<class Value>
int CommParameterRequest::set(const std::string &key, const Value &value) {
  std::stringstream s;
  s << value;
  bool found = false;
  for (size_t i = 0; i < data.items.size(); ++i) {
    if (key.compare(data.items[i].key.c_str()) == 0) {
      found = true;
      data.items[i].values.push_back(s.str().c_str());
      break;
    }
  }
  if (!found) {
    CommParameterIDL::NameValue nv;
    nv.key = key.c_str();
    nv.values.push_back(s.str().c_str());
    data.items.push_back(nv);
  }
  return s.fail() ? -1 : 0;
}

template<class Value>
int CommParameterRequest::get(const std::string &key, Value &value) const {
  bool found = false;
  std::string str_val;
  CommParameterIDL::NameValue nv;
  std::vector<CommParameterIDL::NameValue>::const_iterator it;
  for (it = data.items.begin(); it != data.items.end(); it++) {
    if (key.compare(it->key.c_str()) == 0) {
      found = true;
      str_val = it->values.back().c_str();
      break;
    }
  }
  if (found) {
    std::stringstream s(str_val);
    s >> value;
    return s.fail() ? -1 : 0;
  }
  return -1;
}

template<class Value>
int CommParameterRequest::getList(const std::string &key, std::list<Value> &valueList) const {
  bool found = false;
  std::list<std::string> str_val_list;
  CommParameterIDL::NameValue nv;
  std::vector<CommParameterIDL::NameValue>::const_iterator it;
  for (it = data.items.begin(); it != data.items.end(); it++) {
    if (key.compare(it->key.c_str()) == 0) {
      found = true;
      for (unsigned i = 0; i < it->values.size(); ++i) {
        str_val_list.push_back(it->values[i].c_str());
      }
      break;
    }
  }
  if (found) {
    int result = 0;
    std::list<std::string>::const_iterator sit;
    for (sit = str_val_list.begin(); sit != str_val_list.end(); sit++) {
      Value value;
      std::stringstream s(*sit);
      s >> value;
      if (s.fail()) result = -1;
    }
    return result;
  }
  return -1;
}

template<>
inline int CommParameterRequest::get<std::string>(const std::string &key, std::string &value) const {
  std::string str_val;
  CommParameterIDL::NameValue nv;
  std::vector<CommParameterIDL::NameValue>::const_iterator it;
  for (it = data.items.begin(); it != data.items.end(); it++) {
    if (key.compare(it->key.c_str()) == 0) {
      value = it->values.back().c_str();
      return 0;
    }
  }
  return -1;
}

template<>
inline int CommParameterRequest::getList<std::string>(const std::string &key, std::list<std::string> &valueList) const {
  std::string str_val;
  CommParameterIDL::NameValue nv;
  std::vector<CommParameterIDL::NameValue>::const_iterator it;
  for (it = data.items.begin(); it != data.items.end(); it++) {
    if (key.compare(it->key.c_str()) == 0) {
      for (unsigned i = 0; i < it->values.size(); ++i) {
        valueList.push_back(it->values[i].c_str());
      }
      return 0;
    }
  }
  return -1;
}

} // end namespace DefaultCommObjects
} // end namespace SeRoNet

///
/// \file AsyncSubscriptionWithCommObject.hpp
/// \author Christian von Arnim
/// \date 21.02.2018
///

#pragma once

#include "AsyncSubscriptionOpcUa.hpp"
#include <Open62541Cpp/UA_NodeId.hpp>
#include "Converter/CommObjectBrowseToNodeIds.hpp"
#include "Converter/CommObjectToListOfPrimitivDescriptions.hpp"
#include "Converter/UaVariantArrayToCommObject.hpp"
#include "./../../CommunicationObjects/ICommunicationObject.hpp"

#include <map>
#include <algorithm>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_DATATYPE>
class AsyncSubscriptionWithCommObject : public AsyncSubscriptionOpcUa<T_DATATYPE> {
 public:
  static_assert(
      std::is_base_of<CommunicationObjects::ICommunicationObject, T_DATATYPE>::value,
      "T_DATATYPE must be a descendant of CommunicationObjects::ICommunicationObject"
  );
 protected:
  void processValues(listOfNodeIdValue_t nodeIdvalues) override;
 public:
  using AsyncSubscriptionOpcUa<T_DATATYPE>::AsyncSubscriptionOpcUa;
  /// Subscribe based on an CommunicationObject description
  /// \return UA_GOOD in case of no error
  virtual UA_StatusCode subscribe(open62541::UA_NodeId nodeId);
};

template<typename T_DATATYPE>
UA_StatusCode AsyncSubscriptionWithCommObject<T_DATATYPE>::subscribe(open62541::UA_NodeId nodeId) {
  T_DATATYPE data; // Dummy Data instance to get the description
  CommunicationObjects::ICommunicationObject *pData = &data;
  m_pUaClientWithMutex->pClient;

  auto nodeIds = Converter::CommObjectBrowseToNodeIds(
      pData->getObjectDescription(""),
      nodeId,
      m_pUaClientWithMutex->pClient).getValue();
  auto flatListOfDescriptions =
      Converter::CommObjectToListOfPrimitivDescriptions(
          std::static_pointer_cast<CommunicationObjects::Description::IDescription>(
              pData->getObjectDescription("")
          )
      ).getValue();

  std::map<CommunicationObjects::Description::IDescription *, std::size_t> orderOfFlat;
  // Store the order of the flatList
  {
    std::size_t i = 0;
    for (auto &el: flatListOfDescriptions) {
      orderOfFlat.insert(std::make_pair(el.get(), i++));
    }
  }

  auto compare = [&orderOfFlat](const decltype(nodeIds)::value_type &left,
                                const decltype(nodeIds)::value_type &right) -> bool {
    auto leftIter = orderOfFlat.find(left.Description.get());
    auto rightIter = orderOfFlat.find(left.Description.get());
    if (leftIter == orderOfFlat.end() || rightIter == orderOfFlat.end()) {
      //One or both elements could not be found
      return leftIter != orderOfFlat.end(); // Ensure that these elements will be at the end
    }

    return *leftIter < *rightIter;

  };
  ///reorder nodeIds to match the order of flatListOfDescriptions
  std::sort(nodeIds.begin(), nodeIds.end(), compare);

  //\todo Extract nodeIds and pass to parent class
  std::vector<UA_NodeId> vecOfNodeIds;
  for (auto &nodeIdClassPrimitive : nodeIds) {
    if(nodeIdClassPrimitive.NodeClass == UA_NODECLASS_VARIABLE)
    {
      //Only use variables
      vecOfNodeIds.push_back(*nodeIdClassPrimitive.NodeId.NodeId);
    }
  }
  // Call parent class function
  AsyncSubscriptionOpcUa::subscribe(vecOfNodeIds);
}

template<typename T_DATATYPE>
void AsyncSubscriptionWithCommObject<T_DATATYPE>::processValues(
    AsyncSubscriptionOpcUa<T_DATATYPE>::listOfNodeIdValue_t listOfNodeIdvalues) {
  open62541::UA_ArrayOfVariant variantArray(listOfNodeIdvalues.size());
  std::size_t i = 0;
  for(auto &nodeIdAndValue: listOfNodeIdvalues){
    UA_Variant_copy(nodeIdAndValue->value->DataValue->value, &(variantArray.Variants[i]));
    ++i;
  }
  T_DATATYPE newValue;

  Converter::UaVariantArrayToCommObject conv(variantArray, newValue->getObjectDescription(""));
  this->addData(newValue);
}

}
}
}

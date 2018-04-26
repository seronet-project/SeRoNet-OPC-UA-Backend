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
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

#include <map>
#include <algorithm>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_DATATYPE>
class AsyncSubscriptionWithCommObject : public AsyncSubscriptionOpcUa<T_DATATYPE> {
 public:
  ~AsyncSubscriptionWithCommObject() override = default;
 protected:
  void processValues(typename AsyncSubscriptionOpcUa<T_DATATYPE>::listOfNodeIdValue_t nodeIdvalues) override;
  using AsyncSubscriptionOpcUa<T_DATATYPE>::m_pUaClientWithMutex;
 public:
  using AsyncSubscriptionOpcUa<T_DATATYPE>::AsyncSubscriptionOpcUa;
  /// Subscribe based on an CommunicationObject description
  /// \return UA_GOOD in case of no error
  virtual UA_StatusCode subscribe(open62541::UA_NodeId nodeId);
};

template<typename T_DATATYPE>
UA_StatusCode AsyncSubscriptionWithCommObject<T_DATATYPE>::subscribe(open62541::UA_NodeId nodeId) {
  T_DATATYPE data; // Dummy Data instance to get the description
  std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> lock(m_pUaClientWithMutex->opcuaMutex);

  auto nodeIds = Converter::CommObjectBrowseToNodeIds(
      CommunicationObjects::Description::SelfDescription(&data, ""),
      nodeId,
      m_pUaClientWithMutex->pClient).getValue();
  auto flatListOfDescriptions =
      Converter::CommObjectToListOfPrimitivDescriptions(
          std::static_pointer_cast<CommunicationObjects::Description::IDescription>(
              CommunicationObjects::Description::SelfDescription(&data, "")
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
  nodeIds.sort(compare);
  //std::sort(nodeIds.begin(), nodeIds.end(), compare);

  //\todo Extract nodeIds and pass to parent class
  std::vector<UA_NodeId> vecOfNodeIds;
  for (auto &nodeIdClassPrimitive : nodeIds) {
    if(nodeIdClassPrimitive.NodeClass == UA_NODECLASS_VARIABLE)
    {
      //Only use variables
      vecOfNodeIds.push_back(*nodeIdClassPrimitive.NodeId.NodeId);
    }
  }

  lock.unlock();

  // Call parent class function
  AsyncSubscriptionOpcUa<T_DATATYPE>::subscribe(vecOfNodeIds);
}

template<typename T_DATATYPE>
void AsyncSubscriptionWithCommObject<T_DATATYPE>::processValues(
    typename AsyncSubscriptionOpcUa<T_DATATYPE>::listOfNodeIdValue_t listOfNodeIdvalues) {
  open62541::UA_ArrayOfVariant variantArray(listOfNodeIdvalues.size());
  std::size_t i = 0;
  for(auto &nodeIdAndValue: listOfNodeIdvalues){
    UA_Variant_copy(&(nodeIdAndValue.value->DataValue->value), &(variantArray.Variants[i]));
    ++i;
  }
  T_DATATYPE newValue;

  Converter::UaVariantArrayToCommObject
      conv(variantArray, CommunicationObjects::Description::SelfDescription(&newValue, "").get());
  this->addData(newValue);
}

}
}
}

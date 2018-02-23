///
/// \file CommObjectBrowseToNodeIds.cpp
/// \author Christian von Arnim
/// \date 21.02.2018
///

#include "CommObjectBrowseToNodeIds.hpp"
#include <Open62541Cpp/UA_String.hpp>
#include <../../../CommunicationObjects/Description/ElementPrimitive.hpp>
#include <../../../CommunicationObjects/Description/ComplexType.hpp>
#include <open62541/open62541.h>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

CommObjectBrowseToNodeIds::CommObjectBrowseToNodeIds(
    CommunicationObjects::Description::ComplexType::shp_t complexType,
    const open62541::UA_NodeId &nodeId,
    std::shared_ptr<UA_Client> pClient
) {

  /// Browse for NodeIds, try to find a node for each entry in the description
  UA_BrowseRequest bReq;
  UA_BrowseRequest_init(&bReq);
  bReq.requestedMaxReferencesPerNode = 0;
  bReq.nodesToBrowse = UA_BrowseDescription_new();
  bReq.nodesToBrowseSize = 1;
  UA_NodeId_copy(nodeId.NodeId, &(bReq.nodesToBrowse[0].nodeId));
  bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; /* return everything */

  bReq.nodesToBrowse[0].browseDirection = UA_BROWSEDIRECTION_FORWARD;

  UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);
  printf("%-9s %-16s %-16s %-16s\n", "NAMESPACE", "NODEID", "BROWSE NAME", "DISPLAY NAME");
  for (size_t i = 0; i < bResp.resultsSize; ++i) {
    for (size_t j = 0; j < bResp.results[i].referencesSize; ++j) {
      UA_ReferenceDescription *ref = &(bResp.results[i].references[j]);
      // Use the browse name for comparison
      std::string browseName((const char *) ref->browseName.name.data, (int) ref->browseName.name.length);
      //std::string displayName((const char *) ref->displayName.text.data, (int) ref->displayName.text.length);
      CommunicationObjects::Description::IDescription::shp_t description
          = getDescriptionByName(browseName, complexType);

      if (description == nullptr) {
        // Element not part of the description
        continue;
      }

      ///\todo handle the case of different servers?(ref.nodeId.serverIndex)
      open62541::UA_NodeId nodeIdOfRefTarget(ref->nodeId.nodeId, false);

      if (std::dynamic_pointer_cast<CommunicationObjects::Description::ComplexType>(description) != nullptr) {
        if (ref->nodeClass == UA_NODECLASS_OBJECT) {
          // Recursive call this converter
          CommunicationObjects::Description::ComplexType::shp_t compObj =
              std::dynamic_pointer_cast<CommunicationObjects::Description::ComplexType>(description);
          CommObjectBrowseToNodeIds nodeIds(compObj, nodeIdOfRefTarget, pClient);
          // Move results to this converted value
          m_convertedValue.splice(m_convertedValue.end(), nodeIds.m_convertedValue);
        }
        // if not an object, ignore it
        continue;
      }

        ///\todo search for a check, otherwise add an extra type between IDescription an ElementPrimitive<>,
        /// that alllow these checks
        /*
        if (dynamic_cast<CommunicationObjects::Description::ElementPrimitive *>(description) != nullptr) {

        }
        */
      else {
        if (ref->nodeClass == UA_NODECLASS_VARIABLE) {
          NodeId_NodeClass_Primitive nnp = {nodeIdOfRefTarget, description, ref->nodeClass};
          // add to list
          m_convertedValue.push_back(nnp);
        }
        // if not a variable, ignore it
        continue;
      }

      /*
      if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC) {
        std::string browseName((const char *) ref->browseName.name.data, (int) ref->browseName.name.length);


        std::cout << "Myout: " << browseName << ", " << displayName << std::endl;

        printf("%-9d %-16d %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
               ref->nodeId.nodeId.identifier.numeric, (int) ref->browseName.name.length,
               ref->browseName.name.data, (int) ref->displayName.text.length,
               ref->displayName.text.data);
      } else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING) {
        printf("%-9d %-16.*s %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
               (int) ref->nodeId.nodeId.identifier.string.length,
               ref->nodeId.nodeId.identifier.string.data,
               (int) ref->browseName.name.length, ref->browseName.name.data,
               (int) ref->displayName.text.length, ref->displayName.text.data);
      }
      */
    }
  }
  UA_BrowseRequest_deleteMembers(&bReq);
  UA_BrowseResponse_deleteMembers(&bResp);

  ///\todo check if a nodeId for each primitive description element has been found
}
CommunicationObjects::Description::IDescription::shp_t CommObjectBrowseToNodeIds::getDescriptionByName(
    std::string name,
    CommunicationObjects::Description::ComplexType::shp_t complexType) {
  for (auto &description: *complexType) {
    if (description->getName() == name) {
      return description;
    }
  }
  // No element found
  return nullptr;
}

}
}
}
}

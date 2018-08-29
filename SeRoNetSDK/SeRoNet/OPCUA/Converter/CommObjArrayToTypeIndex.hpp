///
/// \file CommObjArrayToTypeIndex.hpp
/// \author Christian von Arnim
/// \date 03.08.2018
///

#pragma once

#include "../../CommunicationObjects/Description/IDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {
class CommObjArrayToTypeIndex {
 public:
  CommObjArrayToTypeIndex(CommunicationObjects::Description::IDescription *pDescription);
  std::size_t Id();
 protected:
  std::size_t m_id;
};

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet

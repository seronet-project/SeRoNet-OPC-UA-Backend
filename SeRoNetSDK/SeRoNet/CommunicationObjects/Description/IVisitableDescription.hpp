///
/// \file IVisitableDescription.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "IVisitorDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
///\todo ? Merge with IDescription
class IVisitableDescription {
 public:
  virtual void accept(IVisitorDescription *visitor) = 0;
};
}
}
}



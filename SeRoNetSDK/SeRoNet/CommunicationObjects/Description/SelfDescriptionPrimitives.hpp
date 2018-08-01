///
/// \file SelfDescriptionPrimitives.hpp
/// \author Christian von Arnim
/// \date 01.08.2018
///

#pragma once

#include "SelfDescription.hpp"

#include "ElementPrimitives.hpp"
namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

namespace internal {
template<typename PRIMITIVE_TYPE>
inline IDescription::shp_t SelfDescriptionPrimitive(PRIMITIVE_TYPE *ptr, std::string name) {
  auto ret = std::make_shared<ElementPrimitive<PRIMITIVE_TYPE>>(
      name, [ptr]() { return *ptr; }, [ptr](const PRIMITIVE_TYPE &v) { *ptr = v; }
  );
  return ret;
}

}

#define CREATE_SELFDESCR_PRIMITIVE(TYPE) template<> \
IDescription::shp_t SelfDescription(TYPE *ptr, std::string name) {\
  return internal::SelfDescriptionPrimitive<TYPE>(ptr, name);\
}
/*
template<>
IDescription::shp_t SelfDescription(std::int32_t *ptr, std::string name) {
  return internal::SelfDescriptionPrimitive<std::int32_t>(ptr, name);
}*/

CREATE_SELFDESCR_PRIMITIVE(std::int32_t);
CREATE_SELFDESCR_PRIMITIVE(std::string);

}
}
}

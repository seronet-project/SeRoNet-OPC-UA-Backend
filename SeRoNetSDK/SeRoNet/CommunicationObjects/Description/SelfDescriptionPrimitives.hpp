/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

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
inline IDescription::shp_t SelfDescription(TYPE *ptr, std::string name) {\
  return internal::SelfDescriptionPrimitive<TYPE>(ptr, name);\
}
/*
template<>
IDescription::shp_t SelfDescription(std::int32_t *ptr, std::string name) {
  return internal::SelfDescriptionPrimitive<std::int32_t>(ptr, name);
}*/

// TODO create other primitves
CREATE_SELFDESCR_PRIMITIVE(std::int8_t);
CREATE_SELFDESCR_PRIMITIVE(std::uint8_t);
CREATE_SELFDESCR_PRIMITIVE(std::int16_t );
CREATE_SELFDESCR_PRIMITIVE(std::uint16_t);
CREATE_SELFDESCR_PRIMITIVE(std::int32_t);
CREATE_SELFDESCR_PRIMITIVE(std::uint32_t);
CREATE_SELFDESCR_PRIMITIVE(std::int64_t );
CREATE_SELFDESCR_PRIMITIVE(std::uint64_t);
CREATE_SELFDESCR_PRIMITIVE(std::string);
CREATE_SELFDESCR_PRIMITIVE(bool);
CREATE_SELFDESCR_PRIMITIVE(double);
CREATE_SELFDESCR_PRIMITIVE(float);

#undef CREATE_SELFDESCR_PRIMITIVE

template<>
#ifndef SERONET_NO_DEPRECATED
[[deprecated("char is deprecated, use std::int8_t from <cstdint> instead")]]
#endif
inline IDescription::shp_t SelfDescription(char *ptr, std::string name) {
  return internal::SelfDescriptionPrimitive<std::int8_t>((std::int8_t *)ptr, name);
}


}
}
}

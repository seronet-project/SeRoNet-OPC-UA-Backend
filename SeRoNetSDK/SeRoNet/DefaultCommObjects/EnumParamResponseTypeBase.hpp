//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H
#define SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {

enum ENUM_INTERNAL_ParamResponseType {
  LITERAL_BUSY,
  LITERAL_INVALID,
  LITERAL_OK
};
typedef int ENUM_ParamResponseType;

} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end namespace SeRoNet

#endif //SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H

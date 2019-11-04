/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#pragma once

class CoordinateObject :
    public Smart::ICommunicationObject {
 private:
  int x = 0;
  int y = 0;
  int z = 0;

 public:
  bool operator==(const CoordinateObject &rhs) const {
    return x == rhs.x &&
        y == rhs.y &&
        z == rhs.z;
  }
  bool operator!=(const CoordinateObject &rhs) const {
    return !(rhs == *this);
  }
  CoordinateObject() = default;

  CoordinateObject(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

  ~CoordinateObject() override = default;

  inline int getX() const { return x; }

  inline int getY() const { return y; }

  inline int getZ() const { return z; }

  inline void setX(const int &x) { this->x = x; }

  inline void setY(const int &y) { this->y = y; }

  inline void setZ(const int &z) { this->z = z; }

  // implementation of the CommunicationObjectBase interface
  std::string getName() const override {
    return "CoordinateObject";
  }

  void to_ostream(std::ostream &os = std::cout) const {
    os << getName() << "(" << getX() << ", " << getY() << ", " << getZ() << ") ";
  }

};

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
inline IDescription::shp_t SelfDescription<
    CoordinateObject>(CoordinateObject *obj, std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  //  Add Members
  (*ret).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "x",
              std::bind(&CoordinateObject::getX, obj),
              std::bind(&CoordinateObject::setX, obj, std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "y",
              std::bind(&CoordinateObject::getY, obj),
              std::bind(&CoordinateObject::setY, obj, std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementInt32(
              "z",
              std::bind(&CoordinateObject::getZ, obj),
              std::bind(&CoordinateObject::setZ, obj, std::placeholders::_1)
          )
      )
  );
  return ret;
}

}
}
}

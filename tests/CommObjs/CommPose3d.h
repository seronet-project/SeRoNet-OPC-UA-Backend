/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef COMMPOSE3D_H_
#define COMMPOSE3D_H_

#include <string>
#include <iostream>

struct CommPose3d {
	double x;
	double y;
	double z;

//	std::string getName() const {
//		return "CommPose3d";
//	}

  inline bool operator==(const CommPose3d &rhs) const {
	  return x == rhs.x &&
			  y == rhs.y &&
			  z == rhs.z;
  }
  inline bool operator!=(const CommPose3d &rhs) const {
	  return !(rhs == *this);
  }
};

inline std::ostream& operator<<(std::ostream &os, const CommPose3d &obj) {
	os << "CommPose3d(" << "x=" << obj.x << ", y=" << obj.y << ", z=" << obj.z << ")";
	return os;
}

#endif /* COMMPOSE3D_H_ */

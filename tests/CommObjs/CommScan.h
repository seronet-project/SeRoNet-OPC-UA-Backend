/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef COMMSCAN_H_
#define COMMSCAN_H_

#include <vector>
#include <iostream>
#include "CommPose3d.h"

struct CommScan {
	CommPose3d scanner_pos;
	std::vector<double> cov_matrix;
	std::vector<CommPose3d> scan_points;

//	std::string getName() const {
//		return "CommScan";
//	}

  inline bool operator==(const CommScan &rhs) const {
	  return scanner_pos == rhs.scanner_pos &&
			  cov_matrix == rhs.cov_matrix &&
			  scan_points == rhs.scan_points;
  }

  inline bool operator!=(const CommScan &rhs) const {
	  return !(rhs == *this);
  }

};




inline std::ostream& operator<<(std::ostream &os, const CommScan &obj) {
	os << "CommScan(" << "scanner_pos=" << obj.scanner_pos << ", cov_matrix=<";
	for(auto &elem: obj.cov_matrix) {
		os << elem;
		if(&elem != &obj.cov_matrix.back()) {
			os << ", ";
		}
	}
	os << ">, scan_points=<";

	for(auto &point: obj.scan_points) {
		os << point;
		if(&point != &obj.scan_points.back()) {
			os << ", ";
		}
	}
	os << ">)";
	return os;
}

#endif /* COMMSCAN_H_ */

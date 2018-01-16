///
/// @file SeRoNetSDKException.hpp
/// @author Christian von Arnim 
/// @date 05.01.2018
///

#ifndef SERONETSDK_SERONETSDKEXCEPTION_HPP
#define SERONETSDK_SERONETSDKEXCEPTION_HPP


#include <stdexcept>
namespace SeRoNet {
	namespace Exceptions {
		class SeRoNetSDKException : public std::runtime_error {
		public:
			///@todo Add more information
			using std::runtime_error::runtime_error;
		};
	}
}

#endif //SERONETSDK_SERONETSDKEXCEPTION_HPP

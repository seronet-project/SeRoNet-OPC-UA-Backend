///
/// @file BlockingDisabledException.hpp
/// @author Christian von Arnim 
/// @date 05.01.2018
///

#ifndef SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP
#define SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
	namespace Exceptions {
		/// Exception, which is thrown, when an (blocking) operation can not be completed, because blocking is disabled
		class BlockingDisabledException : public SeRoNetSDKException {
		public:
			using SeRoNetSDKException::SeRoNetSDKException;
		};
	}
}

#endif //SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP

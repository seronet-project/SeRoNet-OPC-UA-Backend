///
/// @file UaClientWithMutex.hpp
/// @author Christian von Arnim 
/// @date 10.01.2018
///

#ifndef SERONETSDK_UACLIENTWITHMUTEX_HPP
#define SERONETSDK_UACLIENTWITHMUTEX_HPP

#include <open62541.h>
#include <memory>
#include <mutex>

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {
			struct UaClientWithMutex_t
			{
				std::shared_ptr<UA_Client> pClient;
				std::mutex opcuaMutex;
				typedef std::shared_ptr<UaClientWithMutex_t> shpType;
			};
		}
	}
}


#endif //SERONETSDK_UACLIENTWITHMUTEX_HPP

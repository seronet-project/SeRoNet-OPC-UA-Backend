///
/// @file IBlocking.hpp
/// @author Christian von Arnim 
/// @date 04.01.2018
///

#ifndef SERONETSDK_IBLOCKING_HPP
#define SERONETSDK_IBLOCKING_HPP

#include "InstanceStorage.hpp"
#include <atomic>

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {
			class IBlocking {
			public:
				typedef InstanceStorage<IBlocking> instanceStorage_t;
				///@TODO use shared ptr?! for instStorage
				explicit IBlocking(instanceStorage_t *instStorage, bool blockingEnabled);

				///@return old value of blockingEnabled
				bool enableBlocking();

				///@return old value of blockingEnabled
				bool disableBlocking();

				virtual ~IBlocking();
			protected:
				/// Called when the value of BlockingEnabled is changed
				virtual void blockingChanged() = 0;
				/// ReadOnly m_blockingEnabled
				const std::atomic_bool &BlockingEnabled;
			private:
				std::atomic_bool  m_blockingEnabled = true;
				instanceStorage_t *m_pInstStorage;
				instanceStorage_t::Instance_Container_Iterator_t m_it;
			};
		}
	}
}


#endif //SERONETSDK_IBLOCKING_HPP

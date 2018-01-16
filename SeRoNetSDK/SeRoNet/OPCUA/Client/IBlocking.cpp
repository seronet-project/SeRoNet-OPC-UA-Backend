///
/// @file IBlocking.cpp
/// @author Christian von Arnim 
/// @date 04.01.2018
///

#include "IBlocking.hpp"

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {
			IBlocking::IBlocking(instanceStorage_t *instStorage, bool blockingEnabled)
					: m_pInstStorage(instStorage), m_it(instStorage->add(this)),
					  BlockingEnabled(m_blockingEnabled), m_blockingEnabled(blockingEnabled)
			{
			}

			IBlocking::~IBlocking()
			{
				m_pInstStorage->remove(m_it);
			}

			bool IBlocking::enableBlocking() {
				// Use atomic exchange to prevent race conditions
				bool ret = m_blockingEnabled.exchange(true);
				this->blockingChanged();
				return ret;
			}

			bool IBlocking::disableBlocking() {
				// Use atomic exchange to prevent race conditions
				bool ret = m_blockingEnabled.exchange(false);
				this->blockingChanged();
				return ret;
			}
		}
	}
}


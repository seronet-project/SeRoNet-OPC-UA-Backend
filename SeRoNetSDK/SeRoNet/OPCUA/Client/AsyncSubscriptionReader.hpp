#ifndef ASYNC_SUBSCRIPTIONREADER_HPP
#define ASYNC_SUBSCRIPTIONREADER_HPP

#include <atomic>
#include <condition_variable>
#include <memory>

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {

			template<typename T_DATATYPE>
			class AsyncSubscription;

			template<typename T_DATATYPE>
			class AsyncSubscriptionReader
			{
			public:
				///TODO Asser pSubscription != null
				AsyncSubscriptionReader(std::shared_ptr<AsyncSubscription<T_DATATYPE>> pSubscription) : m_pSubscription(pSubscription) {}

				virtual bool isOverflow() final { return m_overflow; }
				virtual void resetOverflow() final { m_overflow = false; }

				///TODO Check invalid
				bool hasData()
				{
					return m_pSubscription->hasData(m_dataFetchCounter);
				}

				///TODO Check invalid
				/// Blocking Call
				T_DATATYPE getData()
				{
					bool overflow;
					typename AsyncSubscription<T_DATATYPE>::counter_t dataReadCounter;
					T_DATATYPE ret = m_pSubscription->getData(m_dataFetchCounter, overflow, dataReadCounter);
					m_dataFetchCounter = dataReadCounter;
					m_overflow |= overflow;
					return ret;
				}

			private:
				std::shared_ptr<AsyncSubscription<T_DATATYPE>> m_pSubscription;
				typename AsyncSubscription<T_DATATYPE>::counter_t m_dataFetchCounter = 0;
				bool m_overflow = false;
			};
		}
	}
}

#endif

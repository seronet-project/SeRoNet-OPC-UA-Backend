#ifndef ASYNC_SUBSCRIPTION_HPP
#define ASYNC_SUBSCRIPTION_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "AsyncSubscriptionReader.hpp"

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {

			///TODO Add invalid flag
			/// T_DATATYPE must be assignable (operator=) and constructable
			template<typename T_DATATYPE>
			///TODO Add invalid flag
			class AsyncSubscription
			{
			public:
				typedef std::atomic_uint64_t atomic_counter_t;
				typedef std::uint64_t counter_t;

				virtual ~AsyncSubscription() = 0;
			protected:

				//Allow reader to interact with this class
				template<typename U>
				friend class AsyncSubscriptionReader;
				virtual counter_t getDataCounter() = 0;

				virtual void addData(const T_DATATYPE& newData) = 0;

				virtual bool hasData(counter_t dataCounter) = 0;

				virtual T_DATATYPE getData(counter_t dataCounter, bool &overflow, counter_t &readDataCounter) = 0;
			};

			template<typename T_DATATYPE>
			AsyncSubscription<T_DATATYPE>::~AsyncSubscription()
			{
			}
		}
	}
}

#endif

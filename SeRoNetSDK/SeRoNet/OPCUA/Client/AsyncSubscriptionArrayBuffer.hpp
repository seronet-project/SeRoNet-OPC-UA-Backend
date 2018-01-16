#ifndef ASYNC_SUBSCRIPTION_ARRAY_BUFFER_HPP
#define ASYNC_SUBSCRIPTION_ARRAY_BUFFER_HPP

#include "AsyncSubscription.hpp"
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {

			/// T_DATATYPE must be assignable (operator=) and constructable
			template<typename T_DATATYPE>
			class AsyncSubscriptionArrayBuffer : public AsyncSubscription<T_DATATYPE>{
			public:
				AsyncSubscriptionArrayBuffer(std::size_t queueSize = 8) : m_bufferSize(queueSize), m_dataBuffer(new T_DATATYPE[queueSize])
				{
				}

				virtual ~AsyncSubscriptionArrayBuffer() = 0;

			protected:
				virtual typename AsyncSubscription<T_DATATYPE>::counter_t getDataCounter() override{ return m_dataCounter; }

				virtual void addData(const T_DATATYPE& newData) override
				{
					std::unique_lock<decltype(m_dataBufferLock)> lock(m_dataBufferLock);
					++m_dataCounter;
					std::size_t nextIndex = m_dataCounter % m_bufferSize;
					m_dataBuffer[nextIndex] = newData;
					m_cv_hasData.notify_all();
				}

				bool hasData(typename AsyncSubscription<T_DATATYPE>::counter_t dataCounter) { return m_dataCounter != dataCounter; }

				///TODO? Use default arguments (static variables) for overflow and readDatacounter?
				T_DATATYPE getData(
						typename AsyncSubscription<T_DATATYPE>::counter_t dataCounter,
						bool &overflow,
						typename AsyncSubscription<T_DATATYPE>::counter_t &readDataCounter)
				{
					///TODO Assert dataCounter <= m_dataCounter

					std::shared_lock<decltype(m_dataBufferLock)> lock(m_dataBufferLock);
					lock.unlock();
					// Wait for data if no data is availiable
					{	std::mutex cv_m;
						std::unique_lock<decltype(cv_m)> cv_ml(cv_m);
						m_cv_hasData.wait(cv_ml, [&]()-> bool {return this->hasData(dataCounter); });
					}

					lock.lock();

					overflow = (m_dataCounter - dataCounter) > m_bufferSize;

					if (overflow)
					{
						readDataCounter = m_dataCounter - m_bufferSize + 1;
					}
					else
					{
						readDataCounter = dataCounter + 1;
					}
					std::size_t index = readDataCounter % m_bufferSize;
					return m_dataBuffer[index];
				}

			private:
				T_DATATYPE *m_dataBuffer;
				std::shared_mutex m_dataBufferLock;
				const std::size_t m_bufferSize;

				typename AsyncSubscription<T_DATATYPE>::atomic_counter_t m_dataCounter = 0;
				std::condition_variable m_cv_hasData;

				std::atomic_bool m_overflow = false;
			};

			template<typename T_DATATYPE>
			AsyncSubscriptionArrayBuffer<T_DATATYPE>::~AsyncSubscriptionArrayBuffer()
			{
			}
		}
	}
}

#endif

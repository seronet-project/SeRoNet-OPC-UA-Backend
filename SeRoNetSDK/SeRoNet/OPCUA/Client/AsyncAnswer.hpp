#ifndef ASYNC_ANSWER_HPP
#define ASYNC_ANSWER_HPP

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "IBlocking.hpp"
#include "../../Exceptions/BlockingDisabledException.hpp"

namespace SeRoNet {
	namespace OPCUA {
		namespace Client {

			template<typename T_RETURN>
			class AsyncAnswer : IBlocking
			{
			public:
				AsyncAnswer(instanceStorage_t *instStorage, bool blockingEnabled);

			private:
				void blockingChanged() override;

			public:
				virtual ~AsyncAnswer() override;

				bool hasAnswer() { return m_hasAnswer; }
				T_RETURN waitForAnswer();
			protected:

				virtual T_RETURN getAnswer() = 0;
				void setHasAnswer(bool hasAnswer = true) { m_hasAnswer = hasAnswer; m_cv_hasAnswer.notify_all(); }

			private:
				std::atomic_bool m_hasAnswer = false;
				std::condition_variable m_cv_hasAnswer;

			};

			template<typename T_RETURN>
			inline AsyncAnswer<T_RETURN>::AsyncAnswer(instanceStorage_t *instStorage, bool blockingEnabled):
					IBlocking(instStorage, blockingEnabled)
			{
			}

			template<typename T_RETURN>
			inline AsyncAnswer<T_RETURN>::~AsyncAnswer()
			{
			}

			template<typename T_RETURN>
			inline T_RETURN AsyncAnswer<T_RETURN>::waitForAnswer()
			{
				std::mutex cv_m;
				std::unique_lock<decltype(cv_m)> cv_ml(cv_m);
				m_cv_hasAnswer.wait(cv_ml, [&]()-> bool {return this->m_hasAnswer || !BlockingEnabled; });

				if(!this->hasAnswer())
				{
					throw SeRoNet::Exceptions::BlockingDisabledException("No AsyncAnswer available.");
				}
				return getAnswer();
			}

			template<typename T_RETURN>
			void AsyncAnswer<T_RETURN>::blockingChanged() {
				m_cv_hasAnswer.notify_all();
			}

		}
	}
}

#endif

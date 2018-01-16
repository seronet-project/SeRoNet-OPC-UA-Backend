#ifndef ASYNC_ANSWER_FACTORY
#define ASYNC_ANSWER_FACTORY

#include <memory>
#include <open62541.h>
#include "AsyncAnswer.hpp"
#include "InstanceStorage.hpp"
#include "IBlocking.hpp"
namespace SeRoNet {
	namespace OPCUA {
		namespace Client {
			template<typename T_RETURN, typename... T_CAll_ARGS>
			class AsyncAnswerFactory
			{
			public:
				AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient, std::mutex &opcuaThreadMutex);
				virtual ~AsyncAnswerFactory();

				typedef std::shared_ptr<AsyncAnswer<T_RETURN>> CallReturn_t;
				virtual CallReturn_t call(T_CAll_ARGS... args) = 0;

				void disableBlocking();
				void enableBlocking();
			protected:
				std::shared_ptr<UA_Client> m_pClient;
				std::mutex &m_opcuaThreadMutex;
				InstanceStorage<IBlocking> m_activeInstances;
				///Store the blocking state for newly created Instances
				/// @todo move m_activeInstances, m_blockingEnabled, disableBlocking and enableBlocking to base class
				std::atomic_bool m_blockingEnabled = true;
			};

			template<typename T_RETURN, typename... T_CAll_ARGS>
			inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient, std::mutex & opcuaThreadMutex) :
				m_pClient(pClient), m_opcuaThreadMutex(opcuaThreadMutex)
			{

			}


			template<typename T_RETURN, typename... T_CAll_ARGS>
			inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::~AsyncAnswerFactory()
			{
				///@todo Assert instead of output
				if(!m_activeInstances.empty())
				{
					printf("ERROR: Instance Storage is not empty : %s", __FUNCTION__);
				}
			}

			template<typename T_RETURN, typename... T_CAll_ARGS>
			void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::disableBlocking() {
				m_blockingEnabled = false;
				auto callb = [](IBlocking* blocking) -> void {blocking->disableBlocking();};
				m_activeInstances.foreach(callb);
			}

			template<typename T_RETURN, typename... T_CAll_ARGS>
			void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::enableBlocking() {
				m_blockingEnabled = true;
				auto callb = [](IBlocking* blocking) -> void {blocking->enableBlocking();};
				m_activeInstances.foreach(callb);
			}
		}
	}
}

#endif // !ASYNC_ANSWER_FACTORY

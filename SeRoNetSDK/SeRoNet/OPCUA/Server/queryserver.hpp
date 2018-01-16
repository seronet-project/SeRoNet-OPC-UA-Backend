/*
* @author Sebastian Friedl
*/
#ifndef QUERY_SERVER_HPP
#define QUERY_SERVER_HPP

#include <iostream>
#include <signal.h>
#include <mutex>
#include <map>
#include <thread>
#include <open62541.h>
#include "queryserverhandler.hpp"
#include "../../Utils/SmartComponent.hpp"
#include "../../Utils/observerHandler.hpp"


namespace SeRoNet {
	namespace OPCUA {
		namespace Server {

			template<typename T_REQUEST, typename T_ANSWER>
			class QueryServer : public ::SeRoNet::Utils::observerHandler {
			private:

				/// management class of the component
				SeRoNet::Utils::SmartComponent *m_component;
				/// name of service
				std::string m_service;

				std::map<int,T_ANSWER> m_answers;

				static UA_StatusCode methodCallback(
						UA_Server *server,
						const UA_NodeId *sessionId,
						void *sessionContext,
						const UA_NodeId *methodId,
						void *methodContext,
						const UA_NodeId *objectId,
						void *objectContext,
						size_t inputSize,
						const UA_Variant *input,
						size_t outputSize,
						UA_Variant *output);

			public:
				/** Constructor.
				*
				*  Note that a handler has to be supplied. Without a handler, the
				*  QueryServer could not accept a query.
				*
				*  (Currently exception not thrown)
				*
				*  @param component management class of the component
				*  @param service   name of the service
				*/
				QueryServer(SeRoNet::Utils::SmartComponent* component, const std::string& service);

				/** Destructor.
				*  Properly disconnects all service requestors in case of destruction
				*  such that all pending queries are handled correctly at client side
				*  even when the service provider disappears during pending queries.
				*/
				virtual ~QueryServer() throw() = default;

				SeRoNet::Utils::StatusCode runPort();

				/** Provide answer to be sent back to the requestor.
				*
				*  Member function is thread safe and thread reentrant.
				*
				*  @param id identifies the request to which the answer belongs
				*  @param answer is the reply itself.
				*
				*  @return status code:
					*/
				SeRoNet::Utils::StatusCode answer(const int id, const T_ANSWER& answer) throw();

			};

			template<typename T_REQUEST, typename T_ANSWER>
			UA_StatusCode QueryServer<T_REQUEST,T_ANSWER>::methodCallback(
					UA_Server *server,
					const UA_NodeId *sessionId,
					void *sessionContext,
					const UA_NodeId *methodId,
					void *methodContext,
					const UA_NodeId *objectId,
					void *objectContext,
					size_t inputSize,
					const UA_Variant *input,
					size_t outputSize,
					UA_Variant *output)
			{
				QueryServer <T_REQUEST, T_ANSWER> *friendThis = static_cast<QueryServer<T_REQUEST, T_ANSWER>*>(methodContext);

				T_REQUEST *request = new T_REQUEST(input, inputSize);
				int id = rand(); //TODO ersetzten mit std:future
				friendThis->notify(id,(SeRoNet::Utils::ICommunicationObject*) request);
				while (friendThis->m_answers.find(id) == friendThis->m_answers.end()) {
					std::this_thread::yield();
				}
				friendThis->m_answers.at(id).toQueryAnswer(output);
				return UA_STATUSCODE_GOOD;
			}

			template<typename T_REQUEST, typename T_ANSWER>
			inline QueryServer<T_REQUEST, T_ANSWER>::QueryServer(
					SeRoNet::Utils::SmartComponent * component,
					const std::string & service
			) :
					m_component(component),
					m_service(service)
			{
				UA_Server* server = component->getServer();

				int inputSize;
				UA_Argument *inputArguments = NULL;
				T_REQUEST::createAsQueryRequestModell(&inputArguments, inputSize);

				int outputSize;
				UA_Argument *outputArguments = NULL;
				T_ANSWER::createAsQueryAnswerModell(&outputArguments, outputSize);

				UA_MethodAttributes helloAttr;
				UA_MethodAttributes_init(&helloAttr);
				helloAttr.description = UA_LOCALIZEDTEXT("en_US","Component.Query");
				helloAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", this->m_service.c_str());
				helloAttr.executable = true;
				helloAttr.userExecutable = true;
				UA_Server_addMethodNode(
						server,
						UA_NODEID_STRING_ALLOC(1, this->m_service.c_str() ),
						UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
						UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
						UA_QUALIFIEDNAME(1, "hello world"),
						helloAttr,
						&methodCallback,
						inputSize,
						inputArguments,
						outputSize,
						outputArguments,
						this,
						NULL);

			}

			template<typename T_REQUEST, typename T_ANSWER>
			inline  SeRoNet::Utils::StatusCode QueryServer<T_REQUEST, T_ANSWER>::runPort()
			{

				return SeRoNet::Utils::SMART_ERROR;
			}

			template<typename T_REQUEST, typename T_ANSWER>
			inline  SeRoNet::Utils::StatusCode QueryServer<T_REQUEST, T_ANSWER>::answer( int id, const T_ANSWER & answer) throw()
			{
				m_answers.insert(std::pair<int,T_ANSWER>(id, answer));
				return SeRoNet::Utils::SMART_OK;
			}

		}
	}
}

#endif
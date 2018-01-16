/*
* @author Sebastian Friedl
*/

#ifndef PUSH_SERVER_HPP
#define PUSH_SERVER_HPP

#include <iostream>
#include <signal.h>
#include <unordered_map>
#include <open62541.h>
#include "../../Utils/SmartComponent.hpp"



namespace SeRoNet {
	namespace OPCUA {
		namespace Server {

			template <class T_AnswerType>
			class PushServer {
			private:
				/// management class of the component
				SeRoNet::Utils::SmartComponent *m_component;

				/// name of service
				std::string m_service;

			public:
				/** Constructor.
				*
				* @param component  management class of the component
				* @param service    name of the service
				*
				*/
				PushServer(SeRoNet::Utils::SmartComponent* component, const std::string& serviceName);

				/** Destructor.
				*  Properly disconnects all service requestors in case of destruction
				*  such that all connected and subscribed clients are unsubscribed and
				*  disconnected properly.
				*/
				virtual ~PushServer() = default;

				/** Provide new data which is sent to all subscribed clients
				*  taking into account their individual update cycles. Update
				*  cycles are always whole-numbered multiples of the server
				*  update cycle.
				*
				*  (Individual update interval counters are incremented each
				*   time this member function is called irrespectively of the
				*   elapsed time. One should use the time triggered handler to
				*   call the put() member function with the appropriate timing.)
				*
				*  PushHandler::handlePushRequest() directly or indirectly
				*  use this method to provide new data.
				*
				*  @param d contains the newly acquired data to be sent as
				*           update.
				*
				*  @return status code
				*    - SMART_OK                  : everything is ok
				*    - SMART_NOTACTIVATED        : server is stopped and does therefore not
				*                                  distribute any data to clients. In that
				*                                  case update interval counters are not
				*                                  touched.
				*    - SMART_ERROR_COMMUNICATION : communication problems caused by at least
				*                                  one client. The other clients are updated
				*                                  correctly.
				*    - SMART_ERROR               : something went completely wrong with at least one
				*                                  client. Some clients might still been
				*                                  updated correctly.
				*/
				virtual SeRoNet::Utils::StatusCode  put(T_AnswerType d);
			};


			template<class T_AnswerType>
			inline PushServer<T_AnswerType>::PushServer(SeRoNet::Utils::SmartComponent * component, const std::string & serviceName) : m_component(component), m_service(serviceName)
			{
				UA_Server* server = component->getServer();
				T_AnswerType::createPushModell(server);

			}
			
			template<class T_AnswerType>
			inline SeRoNet::Utils::StatusCode PushServer<T_AnswerType>::put(T_AnswerType d)
			{
				d.putObject(this->m_component->getServer());
				return SeRoNet::Utils::SMART_OK;
			}
			

		}
	}
}

#endif
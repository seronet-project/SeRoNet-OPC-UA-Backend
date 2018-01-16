#ifndef QUERY_SERVER_HANDLER_HPP
#define QUERY_SERVER_HANDLER_HPP

#include "SeRoNet/Utils/IObserver.hpp"
#include "queryserver.hpp"


namespace SeRoNet {
	namespace OPCUA {
		namespace Server {

			template<typename T_REQUEST, typename T_ANSWER> class QueryServer;

			template<typename T_REQUEST, typename T_ANSWER>
			class QueryServerHandler : public Utils::IObserver
			{
			protected:
				OPCUA::Server::QueryServer<T_REQUEST, T_ANSWER> *m_server;

			public:
				QueryServerHandler() = default;
				QueryServerHandler(SeRoNet::OPCUA::Server::QueryServer<T_REQUEST, T_ANSWER> *server) : m_server(server) {}

				~QueryServerHandler() = default;


				virtual void handle_input(int id, SeRoNet::Utils::ICommunicationObject *input) {
					auto request = static_cast<T_REQUEST*> (input);
					this->handleQuery(id, request);

				}

				virtual void handleQuery(
					const int id,
					const T_REQUEST *r
				) = 0;

			};

			/*template<typename T_REQUEST, typename T_ANSWER>
			inline void QueryServerHandler<T_REQUEST, T_ANSWER>::handleQuery(
				const int id,
				const T_REQUEST *r
			)
			{
				return;
			}*/
		}
	}
}

#endif
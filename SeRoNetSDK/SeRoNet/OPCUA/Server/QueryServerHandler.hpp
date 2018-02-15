#ifndef QUERY_SERVER_HANDLER_HPP
#define QUERY_SERVER_HANDLER_HPP

////#include "SeRoNet/Utils/IObserver.hpp"
#include "QueryServer.hpp"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIQueryServerPattern_T.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartICommunicationObject.h"


namespace SeRoNet {
    namespace OPCUA {
        namespace Server {

            template<typename T_REQUEST, typename T_ANSWER>
            class QueryServer;

            template<typename T_REQUEST, typename T_ANSWER>
            class QueryServerHandler : public Smart::IQueryServerHandler<T_REQUEST, T_ANSWER, int> {
            protected:
                SeRoNet::OPCUA::Server::QueryServer<T_REQUEST, T_ANSWER> *m_server; //TODO check can be deleted

            public:
                QueryServerHandler() = default;

                QueryServerHandler(SeRoNet::OPCUA::Server::QueryServer<T_REQUEST, T_ANSWER> *server) : m_server
                                                                                                               (server),
                                                                                                       Smart::IQueryServerHandler<T_REQUEST, T_ANSWER, int>(
                                                                                                               server) {}

                ~QueryServerHandler() = default;


                virtual void handle_input(int id, Smart::ICommunicationObject *input) {
                    auto request = static_cast<T_REQUEST *> (input);
                    this->handleQuery(id, request);

                }

                void handleQuery(
                        const int id,
                        const T_REQUEST *r
                ) {
                    return;
                }

            };

        }
    }
}

#endif
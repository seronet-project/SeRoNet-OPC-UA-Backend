//
// Created by Sebastian Friedl on 14.02.2018.
//

#ifndef SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP
#define SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIActiveQueueInputHandlerDecorator_T.h"
#include "Task.hpp"
#include "../OPCUA/Server/QueryServerHandler.hpp"

namespace SeRoNet {
namespace Utils {
template<class InputType>
class ActiveQueueInputHandlerDecoratorImpl
    : public Smart::IActiveQueueInputHandlerDecorator<InputType, SeRoNet::Utils::Task> {
 public:
  ActiveQueueInputHandlerDecoratorImpl(Smart::IComponent *component,
                                       Smart::IInputHandler<InputType> *inner_handler)
      : Smart::IActiveQueueInputHandlerDecorator<InputType, SeRoNet::Utils::Task>(component,
                                                                                  inner_handler) {}

  virtual ~ActiveQueueInputHandlerDecoratorImpl() {}
};

template<class RequestType, class AnswerType>
class ActiveQueueQueryServerHandlerDecoratorImpl
    : public Smart::IActiveQueueQueryServerHandlerDecorator<RequestType, AnswerType, int,
                                                            SeRoNet::Utils::Task>
  //TODO int durch smartID ersetzten
{
 public:
  ActiveQueueQueryServerHandlerDecoratorImpl(Smart::IComponent *component,
                                             OPCUA::Server::QueryServerHandler<RequestType, AnswerType>
                                             *inner_handler)
      : Smart::IActiveQueueQueryServerHandlerDecorator<RequestType, AnswerType, int, SeRoNet::Utils::Task>
            (component,
             inner_handler)        //TODO int durch smartID ersetzten

  {}

  virtual ~ActiveQueueQueryServerHandlerDecoratorImpl() {}
};
}
}

#endif //SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP

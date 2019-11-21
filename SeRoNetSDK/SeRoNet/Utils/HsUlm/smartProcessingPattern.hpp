// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009 Alex Lotz
//
//        lotz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//
//  This file is part of ACE/SmartSoft.
//
//  ACE/SmartSoft is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ACE/SmartSoft is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------------
//FIXME die Datei muss komplett überarbeitet werden

#ifndef _SERONETPROCESSINGPATTERNS_HH
#define _SERONETPROCESSINGPATTERNS_HH

#include <smartIProcessingPatterns_T.h>

#include "../smartMessageQueue.hpp"

#include "../../OPCUA/Server/QueryServerHandler.hpp"
#include "../Task.hpp"
//#include "smartSend.hh"
namespace SeRoNet {
namespace Utils {
namespace HsUlm {

/** Decorator for QueryServerHandler to defer handling to another
 *  thread.
 *
 *  This Decorator (see Design Patterns by Gamma et. al) can be used
 *  if a QueryServerHandler is slow and/or blocking and would
 *  therefore block the framework. This handler is an active object
 *  and has a single thread, in which the Queries are handled one after
 *  another.
 *
 *  example usage:
 *  \code
 *
 *  MySlowQueryHandler: public QueryServerHandler<R,A>
 *  {
 *    ...
 *  };
 *
 *  ...
 *
 *  MySlowQueryHandler slowHandler;
 *  ThreadQueueQueryHandler<R,A> threadedHandler(slowHandler);
 *  QueryServer queryService<R,A>(component,"heavy_computation", threadedHandler);
 *  \endcode
 */
template<class RequestType, class AnswerType>
class ThreadQueueQueryHandler
    : public Smart::IActiveQueryServerHandler<RequestType,AnswerType> 
    , public SeRoNet::Utils::Task
{
private:
	virtual int task_execution() override {
		return this->process_fifo_queue();
	}
public:
	using IQueryServerHandlerPtr = std::shared_ptr<Smart::IQueryServerHandler<RequestType,AnswerType>>;

	/** Create a new threaded QueryServerHandler Decorator.
	 *
	 *  The internal handling thread is started/stopped automatically.
	 *
	 *  @param component          the pointer to the surrounding component
	 *  @param inner_handler_ptr  which will be called in a separate thread.
	 */
	ThreadQueueQueryHandler(Smart::IComponent *component, IQueryServerHandlerPtr inner_handler_ptr)
	:	Smart::IActiveQueryServerHandler<RequestType,AnswerType>(inner_handler_ptr)
	,	SeRoNet::Utils::Task(component)
	{
		SeRoNet::Utils::Task::start();
	}

	virtual ~ThreadQueueQueryHandler()
	{
		SeRoNet::Utils::Task::stop();
	}
};

}// end namespace HsUlm
}// end namespace Utils
}// end namespace SeRoNet


#endif // _SMARTPROCESSINGPATTERNS_HH

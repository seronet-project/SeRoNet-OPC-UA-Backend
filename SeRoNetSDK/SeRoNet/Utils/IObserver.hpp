#pragma once

#include "ICommunicationObject.hpp"

namespace SeRoNet {
	
		namespace Utils {
			/** This template class implements the <b>Observer</b> part of the Observer design pattern for
			*  implementing a generic data-input-handler (i.e. input-data-upcall handler).
			*
			*  This class implements the <b>Observer</b> part of the Observer design pattern.
			*  All the communication-patterns that receive input-data can implement the counterpart
			*  IInputSubject which can be used by this handler for receiving the input-data.
			*  Therefore the abstract method handle_input() needs to be implemented in derived classes.
			*/
			class IObserver {


			public:
				/** This is the main input-handler method that will be automatically called from the given subject
				*  each time the subject receives input-data.
				*
				*  This method should be implemented in derived classes to provide a data-handling procedure.
				*
				*  @param input the input-data reference
				*/
				virtual void handle_input(int queryId, ICommunicationObject* input) = 0;
			};

		}

}
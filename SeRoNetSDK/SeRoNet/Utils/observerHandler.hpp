#include <list>
#include <mutex>
#include "IObserver.hpp"
#include "ICommunicationObject.hpp"

namespace SeRoNet {
		namespace Utils {

			class observerHandler
			{
			private:
				std::list <class IObserver *> m_observers;
				std::mutex observers_mutex;
			public:

				observerHandler() = default;

				virtual void attach(IObserver* aObserver);
				virtual void detach(IObserver* aObserver);
				virtual bool notify(int queryId, ICommunicationObject* aCommunicationObject);
			};

		}
	
}

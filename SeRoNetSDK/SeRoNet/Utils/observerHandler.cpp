#include "observerHandler.hpp"

namespace SeRoNet {
	namespace Utils {


		void observerHandler::attach( IObserver* aObserver)
		{
			std::unique_lock<std::mutex> lock(observers_mutex);
			m_observers.push_back(aObserver);
		}

		void observerHandler::detach(IObserver* aObserver)
		{
			std::unique_lock<std::mutex> lock(observers_mutex);
			m_observers.remove(aObserver);
		}

		bool observerHandler::notify(int queryId, ICommunicationObject* aCommunicationObject)
		{
			std::unique_lock<std::mutex> lock(observers_mutex);
			for (auto& observer : m_observers) {
				observer->handle_input(queryId, aCommunicationObject);
			}
			return !m_observers.empty();
		}
	}
}

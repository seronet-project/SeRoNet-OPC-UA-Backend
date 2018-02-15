///
/// \file IDescription.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "IVisitableDescription.hpp"
#include <memory>

namespace SeRoNet {
	namespace CommunicationObjects {
		namespace Description {
			class IDescription : public IVisitableDescription{
			public:
				typedef std::shared_ptr shp_t;
				explicit IDescription(std::string name): m_name(std::move(name)) {}

				const std::string &getName() const {
					return m_name;
				}

			private:
				const std::string m_name;
			};

		}
	}
}

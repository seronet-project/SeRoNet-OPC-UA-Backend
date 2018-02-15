///
/// \file ComplexType.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "IDescription.hpp"
#include <list>


namespace SeRoNet {
	namespace CommunicationObjects {
		namespace Description {
			class ComplexType : public IDescription{
			public:
				typedef std::list<IDescription::shp_t> ListOfElements_t;
				typedef std::shared_ptr<ComplexType> shp_t;

				using IDescription::IDescription;
				ListOfElements_t::const_iterator begin() {return m_elements.begin();}
				ListOfElements_t::const_iterator end() {return m_elements.end();}
				//Return type allows chain of add (e.g. X.add(...).add(...))
				ComplexType& add(IDescription::shp_t element);

				void accept(IVisitorDescription *visitor) override;

			private:
				ListOfElements_t m_elements;
			};
		}
	}
}

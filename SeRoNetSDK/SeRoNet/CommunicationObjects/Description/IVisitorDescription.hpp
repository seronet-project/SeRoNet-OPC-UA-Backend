///
/// \file IVisitorDescription.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include <cstdint>
#include <string>

namespace SeRoNet {
	namespace CommunicationObjects {
		namespace Description {
			template <typename T>
			class ElementPrimitive<T>;

			class ComplexType;

			class IVisitorDescription {
				virtual void visit(ComplexType* complexObject) = 0;
				virtual void visit(ElementPrimitive<std::int32_t>* el) = 0;
				virtual void visit(ElementPrimitive<std::string>* el) = 0;
				/// \todo declare all types

			};
		}
	}
}

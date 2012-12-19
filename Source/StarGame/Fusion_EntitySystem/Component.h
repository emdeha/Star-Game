#ifndef __COMPONENT_H
#define __COMPONENT_H


#include "ComponentType.h"

namespace FusionEngine
{
	class Component
	{
	private:
		ComponentType type;
		unsigned int bit;

	public:
		Component(ComponentType newType) 
			: type(newType), bit(1 << type) {}
		virtual ~Component() {}

		// Get this component's type.
		ComponentType GetType() const { return type; }

		// Get this component's bit.
		unsigned int GetTypeBit() const { return bit; }
	};
}


#endif
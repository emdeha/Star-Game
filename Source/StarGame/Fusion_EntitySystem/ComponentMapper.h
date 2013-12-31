#ifndef __COMPONENT_MAPPER_H
#define __COMPONENT_MAPPER_H


#pragma warning(push, 1)
#include <vector>
#pragma warning(pop)
#include "Component.h"

namespace FusionEngine
{
	//class Component;

	template<class T>
	class ComponentMapper
	{
	protected:
		const std::vector<Component*> *componentList;

	public:
		ComponentMapper(const std::vector<Component*> &newComponentList) 
			: componentList(&newComponentList) {}
		virtual ~ComponentMapper() {}

		// Get the size of the component list.
		int GetSize() const { return componentList->size(); }

		// Get a specific element of the component list.
		T *operator[](int index)
		{
			return static_cast<T*>((*componentList)[index]);
		}
	};
}


#endif
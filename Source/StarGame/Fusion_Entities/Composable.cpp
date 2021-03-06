#include "stdafx.h"
#include "Composable.h"


using namespace FusionEngine;

void Composable::AddComponent(ComponentType componentID, std::shared_ptr<IComponent> newComponent)
{
	components.insert(std::make_pair(componentID, newComponent));
}

void Composable::RemoveComponent(ComponentType componentID)
{
	auto componentToRemove = components.find(componentID);
	if (componentToRemove != components.end())
	{
		components.erase(componentToRemove);
	}
	else
	{
		std::ostringstream errorMsg;
		errorMsg << "Component with id " << (unsigned int)componentID << " not found!\n";
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	}
}

std::shared_ptr<IComponent> Composable::GetComponent(ComponentType componentID)
{
	auto componentToReturn = components.find(componentID);
	if (componentToReturn != components.end())
	{
		return (*componentToReturn).second;
	}

	// TODO: Figure out how to render skills without this throwing.
	//std::ostringstream errorMsg;
	//errorMsg << "Component with id " << (unsigned int)componentID << " not found!\n";
	//HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}
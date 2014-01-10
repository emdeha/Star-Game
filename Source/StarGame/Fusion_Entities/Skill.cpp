#include "stdafx.h"
#include "Skill.h"

#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Scene/World.h"

#pragma warning(push, 1)
#include <sstream>
#pragma warning(pop)


using namespace FusionEngine;


FusionInput::FusionInput(char newSequenceEndButton,
						 char validInputSequenceA, char validInputSequenceB, char validInputSequenceC,
						 EventManager &eventManager)
{
	validInputButtons[0] = validInputSequenceA;
	validInputButtons[1] = validInputSequenceB;
	validInputButtons[2] = validInputSequenceC;
	sequenceEndButton = newSequenceEndButton;
	eventManager.AddListener(this, FusionEngine::EVENT_ON_KEY_PRESSED);
}

bool FusionInput::IsValidInputButton(char button) const
{
	if (button == validInputButtons[0] ||
	    button == validInputButtons[1] ||
		button == validInputButtons[2])
	{
		return true;
	}

	return false;
}

void FusionInput::AddSequence(const std::string &sequenceName,
							  char buttonA, char buttonB, char buttonC)
{
	//sequences.push_back(FusionSequence(buttonA, buttonB, buttonC));
	sequences.insert(std::make_pair(sequenceName, FusionSequence(buttonA, buttonB, buttonC)));
}

std::string FusionInput::GetSequenceButtons(const std::string &sequenceName) const
{
	for(auto sequence = sequences.begin(); sequence != sequences.end(); ++sequence)
	{
		if((*sequence).first == sequenceName)
		{
			return std::string((*sequence).second.buttons);
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "no such sequence: " << sequenceName;
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return ""; 
}

std::string FusionInput::GetSequenceName(const std::string &fusionButtons) const
{
	for(auto sequence = sequences.begin(); sequence != sequences.end(); ++sequence)
	{
		if((*sequence).second.buttons == fusionButtons)
		{
			return (*sequence).first;
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "no sequence with buttons: " << fusionButtons;
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return ""; 
}

bool FusionInput::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case FusionEngine::EVENT_ON_KEY_PRESSED:
		{
			const OnKeyPressedEvent &data = static_cast<const OnKeyPressedEvent&>(eventData);
			
			if (data.key == sequenceEndButton)
			{
				for (auto sequence = sequences.begin(); sequence != sequences.end(); ++sequence)
				{
					if (currentInputSequence == (*sequence).second.buttons)
					{
						GetWorld().GetEventManager().
							FireEvent(OnFusionCompletedEvent(FusionEngine::EVENT_ON_FUSION_COMPLETED, 
															 (*sequence).first, currentInputSequence));
						break;
					}
				}

				currentInputSequence.resize(0);
				return false;
			}

			if (IsValidInputButton(data.key))
			{
				currentInputSequence += data.key;
			}
			if (currentInputSequence.length() > 3)
			{
				// TODO: Add overwriting logic
				currentInputSequence.clear();
			}
		}
		break;
	}

	return false;
}


/////////////
//  Skill  //
/////////////
Skill::Skill(const std::string &newID,
			 char fusionCombA, char fusionCombB, char fusionCombC,
			 int newApplyCost, int newResearchCost)
			  : Composable(newID),
				applyCost(newApplyCost), researchCost(newResearchCost),
			    OnFusionCompleted(nullptr), OnClick(nullptr), OnUpdate(nullptr)
{
	fusionCombination[0] = fusionCombA;
	fusionCombination[1] = fusionCombB;
	fusionCombination[2] = fusionCombC;
	fusionCombination[3] = '\0';

	GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);
	GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_FUSION_COMPLETED);
}

void Skill::Update()
{
	if (OnUpdate)
	{
		OnUpdate(id);
	}
}

bool Skill::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			if (OnClick)
			{
				OnClick(id, eventData);
			}
		}
		break;
	case FusionEngine::EVENT_ON_FUSION_COMPLETED:
		{
			if (OnFusionCompleted)
			{
				const OnFusionCompletedEvent &data = static_cast<const OnFusionCompletedEvent&>(eventData);

				if (IsForSequence(data.fusionComb))
				{
					OnFusionCompleted(id, eventData);
				}
			}
		}
		break;
	}

	return false;	
}
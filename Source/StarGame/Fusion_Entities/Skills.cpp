#include "stdafx.h"
#include "Skills.h"

#include "../Fusion_Scene/World.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_EntitySystem/EntityEvents.h"

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
															 currentInputSequence));
					}

				}

				currentInputSequence.resize(0);
				return true;
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
bool Skill::HandleEvent(const IEventData &eventData)
{
	return false;	
}

bool Skill::IsForSequence(const std::string &fusionSequence)
{
	return fusionSequence == fusionCombination;
}


/////////////////////////
//  SatelliteCreation  //
/////////////////////////
void SatelliteCreationSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	skillHolder->AddSatellite(satelliteToAdd);
}
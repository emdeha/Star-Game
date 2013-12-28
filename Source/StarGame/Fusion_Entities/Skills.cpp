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


////////////
//  Burn  //
////////////
BurnSkill::BurnSkill(int newDamage, float newRange, float newDuration_seconds, float newDamageApplyTime_seconds,
					 char fusionCombA, char fusionCombB, char fusionCombC,
					 int newApplyCost, int newResearchCost)
						: damage(newDamage), range(newRange), duration_seconds(newDuration_seconds), 
						  damageApplyTime_seconds(newDamageApplyTime_seconds), 
						  damageApplyTimeDuration_seconds(newDamageApplyTime_seconds), position(0.0f), 
						  Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost) 
{
	attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE);
	attackTimer.SetPause(true);

	applicationDisc = Utility::Primitives::Circle(glm::vec4(0.4f, 0.9f, 0.1f, 0.5f), position, range, 90);
	applicationDisc.Init();

	GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);
}

void BurnSkill::Update()
{
	if (isActive)
	{
		attackTimer.Update();
		if (attackTimer.GetTimeSinceStart() > duration_seconds)
		{
			attackTimer.Reset();
			attackTimer.SetPause(true);
			isActive = false;
			isDeployed = false;
		}
		if (attackTimer.GetTimeSinceStart() > damageApplyTime_seconds)
		{
			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, range, damage);
			GetWorld().GetEventManager().FireEvent(_event);

			damageApplyTime_seconds += damageApplyTimeDuration_seconds;
		}

		if (!isDeployed)
		{
			DisplayData currentDisplayData = GetWorld().GetDisplayData();

			glm::vec4 mousePos_atZ =
				GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
															 currentDisplayData.windowHeight,
															 currentDisplayData.projectionMatrix,
															 currentDisplayData.modelMatrix.Top(),
															 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
															 glm::comp::Y);

			position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);
		}
	}
}

void BurnSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	damageApplyTime_seconds = damageApplyTimeDuration_seconds;
	isActive = true;
	isDeployed = false;
	attackTimer.SetPause(true);
}

bool BurnSkill::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
			if (isActive)
			{
				attackTimer.SetPause(false);
				attackTimer.Reset();
				isDeployed = true;
			}
		}
		break;
	}

	return false;
}

void BurnSkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		applicationDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							 GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}


///////////
//  AOE  //
///////////
AOESkill::AOESkill(int newDamage, float newRange,
				   char fusionCombA, char fusionCombB, char fusionCombC,
				   int newApplyCost, int newResearchCost)
				   : damage(newDamage), range(newRange), position(0.0f),
				     Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	applicationDisc = Utility::Primitives::Circle(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), position, range, 90);
	applicationDisc.Init();

	GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);
}

void AOESkill::Update()
{
	if (isActive)
	{
		DisplayData currentDisplayData = GetWorld().GetDisplayData();

		glm::vec4 mousePos_atZ =
			GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
														 currentDisplayData.windowHeight,
														 currentDisplayData.projectionMatrix,
														 currentDisplayData.modelMatrix.Top(),
														 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
														 glm::comp::Y);

		position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);
	}
	if (isDeployed)
	{
		// Update animation
	}
}

void AOESkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	isActive = true;
}

bool AOESkill::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);

			if (isActive)
			{
				isDeployed = true; // set to false on anim end
				isActive = false;

				OnSkillAppliedEvent _event = 
					OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, range, damage);
				GetWorld().GetEventManager().FireEvent(_event);
			}
		}
		break;
	}

	return false;
}

void AOESkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		applicationDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							 GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
	if (isDeployed)
	{
		// Render animation
	}
}


//////////////////////////
//  Satellite Creation  //
//////////////////////////
void SatelliteCreationSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	skillHolder->AddSatellite(satelliteToAdd);
}


////////////////
//  Ultimate  //
////////////////
void UltimateSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, glm::vec3(), -1.0f, 300));
}
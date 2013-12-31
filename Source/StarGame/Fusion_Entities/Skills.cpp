#include "stdafx.h"
#include "Skills.h"

#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_Scene/Scene.h"

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
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, range, damage, false);
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
					OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, range, damage, false);
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


///////////////////
//  Passive AOE  //
///////////////////
PassiveAOESkill::PassiveAOESkill(int newDamage, float newRange, float newDamageApplyTime_seconds, float skillLife,
								 char fusionCombA, char fusionCombB, char fusionCombC,
								 int newApplyCost, int newResearchCost)
								 : damage(newDamage), range(newRange), 
								   damageApplyTime_seconds(newDamageApplyTime_seconds), position(0.0f),
								   Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE, damageApplyTime_seconds);
	skillLifeTimer = Framework::Timer(Framework::Timer::TT_SINGLE, skillLife);

	skillVisibilityDisc = Utility::Primitives::Circle(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), position, range, 90);
	skillVisibilityDisc.Init();
}

void PassiveAOESkill::Update()
{
	if (isActive)
	{
		if (!skillLifeTimer.Update())
		{
			attackTimer.Update();
			if (attackTimer.GetTimeSinceStart() >= damageApplyTime_seconds)
			{
				OnSkillAppliedEvent _event = 
					OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, range, damage, false);
				GetWorld().GetEventManager().FireEvent(_event);

				attackTimer.Reset();
			}
			// update anim
		}
		else
		{
			isActive = false;
			skillLifeTimer.Reset();
			skillLifeTimer.SetPause(true);
		}
	}
}

void PassiveAOESkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	isActive = true;
	skillLifeTimer.SetPause(false);

	ComponentMapper<Transform> sunTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_TRANSFORM);

	position = sunTransformData[0]->position;
}

void PassiveAOESkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		skillVisibilityDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
								 GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}


////////////////
//  Sun Nova  //
////////////////
SunNovaSkill::SunNovaSkill(int newDamage, float newRange, float newScaleRate,
						   char fusionCombA, char fusionCombB, char fusionCombC,
						   int newApplyCost, int newResearchCost)
						   : damage(newDamage), range(newRange), scaleRate(newScaleRate), 
						     position(0.0f), currentScale(1.0f),
						     Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	novaExpansionDisc = 
		Utility::Primitives::Torus2D(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
	novaExpansionDisc.Init();
}

void SunNovaSkill::Update()
{
	if (isActive)
	{
		if (currentScale <= range)
		{
			currentScale += scaleRate;

			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, currentScale, damage, true);
			GetWorld().GetEventManager().FireEvent(_event);
		}
		else
		{
			currentScale = 1.0f;
			isActive = false;
		}
	}
}

void SunNovaSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	isActive = true;

	ComponentMapper<Transform> sunTransformData =
		GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_TRANSFORM);

	position = sunTransformData[0]->position;
}

void SunNovaSkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);
		GetWorld().GetDisplayData().modelMatrix.Scale(currentScale, 0.0f, currentScale);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		novaExpansionDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							   GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}


//////////////
//  Shield  //
//////////////
ShieldSkill::ShieldSkill(int newDefensePoints, float newRange,
						 char fusionCombA, char fusionCombB, char fusionCombC,
						 int newApplyCost, int newResearchCost)
						 : defensePoints(newDefensePoints), range(newRange), currentDefensePoints(newDefensePoints), 
						   position(0.0f),
						   Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	skillEffectDisc = Utility::Primitives::Circle(glm::vec4(0.5f, 0.5f, 1.0f, 0.5f), position, range, 90);
	skillEffectDisc.Init();

	GetWorld().GetEventManager().AddListener(this, EVENT_ON_COLLIDE);
}

void ShieldSkill::Update()
{
	if (isActive)
	{
		if (defensePoints == 0)
		{
			isActive = false;
			currentDefensePoints = defensePoints;
		}
		else
		{
			ComponentMapper<Transform> holderTransformData =
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(holderID), CT_TRANSFORM);

			position = holderTransformData[0]->position;
		}
	}
}

void ShieldSkill::Activate(CelestialBody *skillHolder)
{
	holderID = skillHolder->GetID();
	isActive = true;
}

bool ShieldSkill::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_COLLIDE:
		{
			defensePoints--;
		}
		break;
	}

	return false;
}

void ShieldSkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		skillEffectDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							 GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}


/////////////
//  Frost  //
/////////////
FrostSkill::FrostSkill(int newDamage, float newRange, float newScaleRate,
					   char fusionCombA, char fusionCombB, char fusionCombC,
					   int newApplyCost, int newResearchCost)
						: damage(newDamage), range(newRange), scaleRate(newScaleRate), 
						  position(0.0f), currentScale(0.5f),
						  Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	frostExpansionDisc = 
		Utility::Primitives::Torus2D(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
	frostExpansionDisc.Init();
}

void FrostSkill::Update()
{
	if (isActive)
	{
		if (currentScale <= range)
		{
			currentScale += scaleRate;

			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, currentScale, damage, true);
			GetWorld().GetEventManager().FireEvent(_event);

			ComponentMapper<Transform> holderTransformData =
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(holderID), CT_TRANSFORM);

			position = holderTransformData[0]->position;
		}
		else
		{
			currentScale = 1.0f;
			isActive = false;
		}
	}
}

void FrostSkill::Activate(FusionEngine::CelestialBody *skillHolder)
{
	holderID = skillHolder->GetID();
	isActive = true;
}

void FrostSkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);
		GetWorld().GetDisplayData().modelMatrix.Scale(currentScale, 0.0f, currentScale);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		frostExpansionDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							    GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}


///////////////////
//  Chain Skill  //
///////////////////
ChainSkill::ChainSkill(int newDamage, float newRange, float newScaleRate,
					   char fusionCombA, char fusionCombB, char fusionCombC,
					   int newApplyCost, int newResearchCost)
						: damage(newDamage), range(newRange), scaleRate(newScaleRate), 
						  position(0.0f), currentScale(0.5f),
						  Skill(fusionCombA, fusionCombB, fusionCombC, newApplyCost, newResearchCost)
{
	chainExpansionDisc = 
		Utility::Primitives::Torus2D(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
	chainExpansionDisc.Init();

	GetWorld().GetEventManager().AddListener(this, EVENT_ON_SKILL_APPLIED);
}

void ChainSkill::Update()
{
	if (isActive)
	{
		if (currentScale <= range)
		{
			currentScale += scaleRate;

			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, position, currentScale, damage, true);
			GetWorld().GetEventManager().FireEvent(_event);

			ComponentMapper<Transform> holderTransformData =
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(holderID), CT_TRANSFORM);

			position = holderTransformData[0]->position;
		}
		else
		{
			currentScale = 1.0f;
			isActive = false;
		}
	}
}

void ChainSkill::Activate(CelestialBody *skillHolder)
{
	holderID = skillHolder->GetID(); // Bad stuff. Maybe it'd be better if there's just a method for setting the holderID.
}

bool ChainSkill::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_SKILL_APPLIED:
		{
			const OnSkillAppliedEvent &data = static_cast<const OnSkillAppliedEvent&>(eventData);

			ComponentMapper<Transform> holderTransformData =
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(holderID), CT_TRANSFORM);

			if (data.isNova)
			{
				if (glm::length(data.position - holderTransformData[0]->position) < data.radius &&
					glm::length(data.position - holderTransformData[0]->position) >= data.radius - 0.1f)
				{
					isActive = true;
					position = holderTransformData[0]->position;
				}
			}
		}
		break;
	}

	return false;
}

void ChainSkill::Render()
{
	if (isActive)
	{
		glutil::PushStack push(GetWorld().GetDisplayData().modelMatrix);
		GetWorld().GetDisplayData().modelMatrix.Translate(position);
		GetWorld().GetDisplayData().modelMatrix.Scale(currentScale, 0.0f, currentScale);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		chainExpansionDisc.Draw(GetWorld().GetDisplayData().modelMatrix, 
							    GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
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
	GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, glm::vec3(), -1.0f, 300, false));
}
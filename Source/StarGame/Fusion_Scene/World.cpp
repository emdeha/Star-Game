#include "stdafx.h"
#include "World.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_GUI/ControlEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Entities/Scripts.h"

using namespace FusionEngine;

#define FAST_LOAD

World::World()
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

World::~World()
{
}

void World::CreateSun()
{
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject sunMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");
	
	sun = std::make_shared<CelestialBody>(FE_SUN, 4, 0.5f, 0.0f);

	std::shared_ptr<FusionEngine::RenderComponent> sunRenderComponent = std::make_shared<RenderComponent>();
	sunRenderComponent->vao = sunMesh.vao;
	sunRenderComponent->shaderProgramID = shaderManager.GetProgram(FE_PROGRAM_SIMPLE).programId;
	sunRenderComponent->renderType = RenderComponent::FE_RENDERER_SIMPLE;
	
	sun->AddComponent(FE_COMPONENT_RENDER, sunRenderComponent);

	std::shared_ptr<FusionEngine::TransformComponent> sunTransformComponent = std::make_shared<TransformComponent>();
	sunTransformComponent->position = glm::vec3();
	sunTransformComponent->scale = glm::vec3(0.5f);
	sunTransformComponent->rotation = glm::vec3();

	sun->AddComponent(FE_COMPONENT_TRANSFORM, sunTransformComponent);

	renderer.SubscribeForRendering("sun", sunMesh);
}

void World::CreateEnemy(const std::string &id, glm::vec3 position)
{
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject enemyMesh = meshLoader.LoadAssetObject("mesh-files", "spaceship.obj");

	std::shared_ptr<FusionEngine::RenderComponent> spaceshipRenderComponent = std::make_shared<RenderComponent>();
	spaceshipRenderComponent->vao = enemyMesh.vao;
	spaceshipRenderComponent->shaderProgramID = shaderManager.GetProgram(FE_PROGRAM_LIT).programId;
	spaceshipRenderComponent->renderType = RenderComponent::FE_RENDERER_LIT;
	
	std::shared_ptr<FusionEngine::TransformComponent> spaceshipTransformComponent = 
		std::make_shared<TransformComponent>();
	spaceshipTransformComponent->position = position;
	spaceshipTransformComponent->rotation = glm::vec3();
	spaceshipTransformComponent->scale = glm::vec3(0.1f);

	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // TODO: Make relative to the Sun

	std::shared_ptr<Enemy> firstEnemy = std::make_shared<Enemy>(id, 0.02f, frontVector);
	firstEnemy->AddComponent(FE_COMPONENT_RENDER, spaceshipRenderComponent);
	firstEnemy->AddComponent(FE_COMPONENT_TRANSFORM, spaceshipTransformComponent);
	enemies.push_back(firstEnemy);

	renderer.SubscribeForRendering(id, enemyMesh);
}

void World::CreateSkill(const std::string &skillID, const std::string &skillFusionCombination,
						bool hasGeneric, int damage, float range, glm::vec3 position,
						CelestialBodyType bodyToCreate,
						OnEventFunc skillOnClick, OnEventFunc skillOnFusionCompleted, OnUpdateFunc skillOnUpdate,
						bool isSelectorApplied, glm::vec4 selectorColor, bool hasTransform,
						bool isTimed, float duration_secs, float damageDuration_secs, float skillLife_secs,
						const std::string &holderID,
						bool isAnimated, float currentScale, float scaleRate)
{
	std::shared_ptr<Skill> newSkill = 
		std::shared_ptr<Skill>(new Skill(skillID, skillFusionCombination[0], 
												  skillFusionCombination[1], 
												  skillFusionCombination[2], 0, 0));

	if (skillOnClick)
	{
		newSkill->SetOnClickCallback(skillOnClick);
	}
	if (skillOnFusionCompleted)
	{
		newSkill->SetOnFusionCompletedCallback(skillOnFusionCompleted);
	}
	if (skillOnUpdate)
	{
		newSkill->SetOnUpdateCallback(skillOnUpdate);
	}

	if (hasGeneric)
	{
		std::shared_ptr<SkillGenericComponent> newGeneric = std::make_shared<SkillGenericComponent>();
		newGeneric->holderID = holderID;
		newGeneric->damage = damage;
		newGeneric->range = range;
		newGeneric->isActive = false;
		newGeneric->isDeployed = false;
		newGeneric->isChain = false;

		newSkill->AddComponent(FE_COMPONENT_SKILL_GENERIC, newGeneric);

		std::shared_ptr<TransformComponent> newTransform = std::make_shared<TransformComponent>();
		newTransform->position = position;
		newTransform->scale = glm::vec3();
		newTransform->rotation = glm::vec3();

		newSkill->AddComponent(FE_COMPONENT_TRANSFORM, newTransform);
	}

	if (isSelectorApplied)
	{
		std::shared_ptr<SkillSelectorAppliedComponent> newSelectorApplied = 
			std::make_shared<SkillSelectorAppliedComponent>();
		Utility::Primitives::Circle selector = Utility::Primitives::Circle(selectorColor, position, range, 90);
		selector.Init();
		newSelectorApplied->skillSelector = selector;
		
		newSkill->AddComponent(FE_COMPONENT_SKILL_SELECTOR_APPLIED, newSelectorApplied);
	}

	if (isTimed)
	{
		std::shared_ptr<SkillTimedComponent> newSkillTimed = std::make_shared<SkillTimedComponent>();

		newSkillTimed->attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE);
		newSkillTimed->attackTimer.SetPause(true);
		if (skillLife_secs > 0.0f)
		{
			newSkillTimed->skillLifeTimer = Framework::Timer(Framework::Timer::TT_SINGLE, skillLife_secs);
		}

		newSkillTimed->damageApplyTimeDuration_seconds = damageDuration_secs;
		newSkillTimed->damageApplyTime_seconds = damageDuration_secs;
		newSkillTimed->duration_seconds = duration_secs;

		newSkill->AddComponent(FE_COMPONENT_SKILL_TIMED, newSkillTimed);
	}

	if (isAnimated)
	{
		std::shared_ptr<SkillAnimatedComponent> newSkillAnimated = std::make_shared<SkillAnimatedComponent>();

		newSkillAnimated->currentScale = currentScale;
		newSkillAnimated->scaleRate = scaleRate;

		Utility::Primitives::Torus2D anim =
			Utility::Primitives::Torus2D(selectorColor, position, currentScale, currentScale+0.02f, 90);
		anim.Init();

		newSkillAnimated->anim = anim;

		newSkill->AddComponent(FE_COMPONENT_SKILL_ANIMATED, newSkillAnimated);
	}

	if (hasTransform)
	{
		std::shared_ptr<TransformComponent> newTransform = std::make_shared<TransformComponent>();
		newTransform->position = position;
		newTransform->scale = glm::vec3(range, 0.0f, range);
		newTransform->rotation = glm::vec3();
		
		newSkill->AddComponent(FE_COMPONENT_TRANSFORM, newTransform);
	}

	if (bodyToCreate != FE_CELESTIAL_BODY_BAD)
	{
		std::shared_ptr<SkillSatelliteCreationComponent> newSatCreation = 
			std::make_shared<SkillSatelliteCreationComponent>();
		newSatCreation->satelliteType = bodyToCreate;
		newSkill->AddComponent(FE_COMPONENT_SKILL_SATELLITE_CREATION, newSatCreation);
	}

	sun->AddSkill(skillID, newSkill);
}

void World::Load(const std::string &guiLayoutFile,
				 const std::string &audioFile,
				 const std::string &shaderDataFile)
{
	// Load Shader Programs
	FusionEngine::AssetLoader<FusionEngine::ShaderAssetObject> shaderLoader;
	shaderLoader.RegisterType("loader-files", new FusionEngine::ShaderLoader());
	FusionEngine::ShaderAssetObject loadedShaders = shaderLoader.LoadAssetObject("loader-files", shaderDataFile);
	shaderManager.Load(loadedShaders.GetAllLoadedPrograms(),
					   loadedShaders.GetAllLoadedUniformBuffers(),
					   loadedShaders.GetAllLoadedBlockIndices());

	// Load GUI
	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
	guiLayouts = loadedGUI.GetAllLoadedLayouts();

#ifndef FAST_LOAD
	// Load Audio
	FusionEngine::AssetLoader<FusionEngine::AudioAssetObject> audioLoader;
	audioLoader.RegisterType("loader-files", new FusionEngine::CAudioLoader());
	FusionEngine::AudioAssetObject loadedAudio = audioLoader.LoadAssetObject("loader-files", audioFile);
	auto loadedAudios = loadedAudio.GetAllLoadedAudios();
	
	std::string path = "../data/music/"; // TODO: Get from config
	for (auto loadedAudio = loadedAudios.begin(); loadedAudio != loadedAudios.end(); ++loadedAudio)
	{
		for (auto audioFile = loadedAudio->second.audioFiles.begin(); 
			 audioFile != loadedAudio->second.audioFiles.end(); ++audioFile)
		{
			path += audioFile->path;
			audio.SetFileForPlay(path, audioFile->soundType, audioFile->isLooping);
			audio.SetVolume(loadedAudio->second.channelVolume, loadedAudio->second.channel);
			path = "../data/music/";
		}
	}
#endif
	
	// Load Cheats

	// Create Test Entities
	CreateSun();
	CreateEnemy("sp2", glm::vec3(0.0f, 0.0f, 6.0f));
	CreateEnemy("sp1", glm::vec3(4.0f, 0.0f, 0.0f));

	CreateSkill("waterSat", "www", false, 0, 0.0f, glm::vec3(),
				FE_WATER_SAT, nullptr, SatelliteCreation_OnFusionCompleted, nullptr);
	CreateSkill("airSat", "qqq", false, 0, 0.0f, glm::vec3(),
				FE_AIR_SAT, nullptr, SatelliteCreation_OnFusionCompleted, nullptr);
	CreateSkill("fireSat", "qwe", false, 0, 0.0f, glm::vec3(),
				FE_FIRE_SAT, nullptr, SatelliteCreation_OnFusionCompleted, nullptr);
	CreateSkill("earthSat", "eee", false, 0, 0.0f, glm::vec3(),
				FE_EARTH_SAT, nullptr, SatelliteCreation_OnFusionCompleted, nullptr);

	CreateSkill("ult", "ewq", true, 300, -1.0f, glm::vec3(), FE_CELESTIAL_BODY_BAD,
				nullptr, Ultimate_OnFusionCompleted, nullptr);
	CreateSkill("aoe", "wqe", true, 10, 2.0f, glm::vec3(), FE_CELESTIAL_BODY_BAD,
				AOE_OnClick, AOE_OnFusionCompleted, AOE_OnUpdate,
				true, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), true);
	CreateSkill("burn", "wee", true, 10, 2.0f, glm::vec3(), FE_CELESTIAL_BODY_BAD,
				Burn_OnClick, Burn_OnFusionCompleted, Burn_OnUpdate,
				true, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), true,
				true, 3.0f, 1.0f);
	CreateSkill("passiveAoe", "weq", true, 10, 2.0f, glm::vec3(), FE_CELESTIAL_BODY_BAD,
				nullptr, PassiveAOE_OnFusionCompleted, PassiveAOE_OnUpdate,
				true, glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), true,
				true, 10.0f, 1.0f, 3.0f);

	CreateSkill("sunNova", "wqq", true, 50, 5.0f, glm::vec3(), FE_CELESTIAL_BODY_BAD,
				nullptr, SunNova_OnFusionCompleted, SunNova_OnUpdate, false, 
				glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), true, false, 0.0f, 0.0f,
				0.0f, "sun", true, 1.0f, 0.02f);
	// ***************************************
	// Skill Chain added on satellite creation
	// ***************************************
}

void World::ReloadGUI(const std::string &guiLayoutFile)
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		(*layout).second->RemoveListeners(eventManager);
	}
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);

	guiLayouts.clear();

	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
	guiLayouts = loadedGUI.GetAllLoadedLayouts();
}

void World::Update()
{
	sun->Update();
	for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
	{
		(*enemy)->Update();
	}
}

void World::Render()
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			(*layout).second->Draw(displayData.modelMatrix);
			
			if ((*layout).first == FE_LAYOUT_GAME)
			{
				sunLight.Render(displayData.modelMatrix, shaderManager);
				renderer.Render(displayData.modelMatrix);
				
				auto skills = sun->GetAllSkills();
				for (auto skill = skills.begin(); skill != skills.end(); ++skill)
				{
					(*skill)->Render();
				}
			}
		}
	}
}

void World::SetLayout(LayoutType layoutToSet)
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).first == layoutToSet)
		{
			(*layout).second->Set(true);
		}
		else 
		{
			(*layout).second->Set(false);
		}
	}
}

Layout *World::GetLayout(LayoutType layoutType) const
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->GetType() == layoutType)
		{
			return (*layout).second.get();
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "no such layout: " << (int)layoutType;
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}

Layout *World::GetCurrentLayout() const
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			return (*layout).second.get();
		}
	}

	HandleUnexpectedError("no current layout", __LINE__, __FILE__);
	return nullptr;
}

unsigned int World::GetCurrentFusionInputIndex() const
{
	return fusionInput->GetCurrentInputIndex();
}

std::string World::GetFusionInputValidButtons() const
{
	return fusionInput->GetValidInputButtons();
}
char World::GetFusionInputSequenceEndButton() const
{
	return fusionInput->GetSequenceEndButton();
}

void World::SetFusionInput(char sequenceEndButton,
						   char validButtonA, char validButtonB, char validButtonC)
{
	fusionInput = std::unique_ptr<FusionInput>(
		new FusionInput(sequenceEndButton, validButtonA, validButtonB, validButtonC,
					    eventManager));
}

void World::AddFusionSequence(const std::string &sequenceName,
							  char buttonA, char buttonB, char buttonC)
{
	fusionInput->AddSequence(sequenceName, buttonA, buttonB, buttonC);
}
void World::AddFusionSequence(const std::string &sequenceName,
							  const std::string &sequenceButtons)
{
	AddFusionSequence(sequenceName, sequenceButtons[0], sequenceButtons[1], sequenceButtons[2]);
}

std::string World::GetActiveSkillName() const
{
	if (fusionInput->GetCurrentInputSequence().length() > 0)
	{
		return fusionInput->GetSequenceName(fusionInput->GetCurrentInputSequence());
	}
	else return "";
}

std::shared_ptr<IComponent> World::GetComponentForObject(const std::string &id, ComponentType componentID)
{
	if (id == "sun")
	{
		return sun->GetComponent(componentID);
	}
	else
	{
		auto sunSats = sun->GetSatellites();
		for (auto sat = sunSats.begin(); sat != sunSats.end(); ++sat)
		{
			if ((*sat)->GetID() == id)
			{
				return (*sat)->GetComponent(componentID);
			}
		}

		auto allSkills = sun->GetAllSkills();
		for (auto skill = allSkills.begin(); skill != allSkills.end(); ++skill)
		{
			if ((*skill)->GetID() == id)
			{
				return (*skill)->GetComponent(componentID);
			}
		}

		for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
		{
			if ((*enemy)->GetID() == id)
			{
				return (*enemy)->GetComponent(componentID);
			}
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "Object with id: " << id << " not found";
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}

bool World::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case FusionEngine::EVENT_ON_RESHAPE:
		{
			const OnReshapeEvent &data = static_cast<const OnReshapeEvent&>(eventData);
			displayData.windowHeight = data.windowHeight;
			displayData.windowWidth = data.windowWidth;
		}
		break;
	}

	return false;
}
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

	// WARN: Sequential calls to AddSatellite make sequential calls to srand(time()) 
	//       which breaks the random generation algorithm
	//sun->AddSatellite(FE_FIRE_SAT);
	//sun->AddSatellite(FE_AIR_SAT);
	//sun->AddSatellite(FE_WATER_SAT);
	//sun->AddSatellite(FE_EARTH_SAT);
}

void World::CreateEnemy(const std::string &enemyID, glm::vec3 position)
{
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject enemyMesh = meshLoader.LoadAssetObject("mesh-files", "spaceship.obj");

	std::shared_ptr<FusionEngine::RenderComponent> spaceshipRenderComponent = std::make_shared<RenderComponent>();
	spaceshipRenderComponent->vao = enemyMesh.vao;
	spaceshipRenderComponent->shaderProgramID = shaderManager.GetProgram(FE_PROGRAM_LIT).programId;
	spaceshipRenderComponent->renderType = RenderComponent::FE_RENDERER_LIT;
	
	std::shared_ptr<FusionEngine::TransformComponent> spaceshipTransformComponent = std::make_shared<TransformComponent>();
	spaceshipTransformComponent->position = position;
	spaceshipTransformComponent->rotation = glm::vec3();
	spaceshipTransformComponent->scale = glm::vec3(0.1f);

	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // TODO: Make relative to the Sun

	std::shared_ptr<Enemy> firstEnemy = std::make_shared<Enemy>(enemyID, 0.02f, frontVector);
	firstEnemy->AddComponent(FE_COMPONENT_RENDER, spaceshipRenderComponent);
	firstEnemy->AddComponent(FE_COMPONENT_TRANSFORM, spaceshipTransformComponent);
	enemies.push_back(firstEnemy);

	renderer.SubscribeForRendering(enemyID, enemyMesh);
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

	// Load Entities
	CreateSun();
	CreateEnemy("spaceship1", glm::vec3(0.0f, 0.0f, 6.0f));
	CreateEnemy("spaceship2", glm::vec3(4.0f, 0.0f, 0.0f));

	// Skill
	std::shared_ptr<Skill> waterSat = std::shared_ptr<Skill>(new Skill("waterSat", 'w', 'w', 'w', 0, 0));
	waterSat->SetOnFusionCompletedCallback(SatelliteCreation_OnFusionCompleted);
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
				/*
				// | This is one ugly motherfucker |
				// V							   V
				ComponentMapper<Updatable> functionalData = 
					GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_UPDATABLE_BEHAVIOR);
				CelestialBody *sun = static_cast<CelestialBody*>(functionalData[0]->updatedObject.get());
				auto sunSkills = sun->GetSkills();
				for (auto skill = sunSkills.begin(); skill != sunSkills.end(); ++skill)
				{
					(*skill)->Render();
				}

				auto sunSats = sun->GetSatellites();
				for (auto sat = sunSats.begin(); sat != sunSats.end(); ++sat)
				{
					auto satSkills = (*sat)->GetSkills();
					for (auto satSkill = satSkills.begin(); satSkill != satSkills.end(); ++satSkill)
					{
						(*satSkill)->Render();
					}
				}
				// | End of ugly motherfucker |*/
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
/*
std::vector<std::shared_ptr<Skill>> World::GetCollidableSkills() const
{
	std::vector<std::shared_ptr<Skill>> collidableSkills;

	// | This is the other ugly motherfucker |
	// V							         V
	ComponentMapper<Updatable> functionalData = 
		GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_UPDATABLE_BEHAVIOR);
	CelestialBody *sun = static_cast<CelestialBody*>(functionalData[0]->updatedObject.get());
	auto sunSkills = sun->GetSkills();
	for (auto skill = sunSkills.begin(); skill != sunSkills.end(); ++skill)
	{
		if ((*skill)->IsCollidable())
		{
			collidableSkills.push_back((*skill));
		}
	}

	auto sunSats = sun->GetSatellites();
	for (auto sat = sunSats.begin(); sat != sunSats.end(); ++sat)
	{
		auto satSkills = (*sat)->GetSkills();
		for (auto satSkill = satSkills.begin(); satSkill != satSkills.end(); ++satSkill)
		{
			if ((*satSkill)->IsCollidable())
			{
				collidableSkills.push_back((*satSkill));
			}
		}
	}
	// | End of other ugly motherfucker |

	return collidableSkills;
}*/

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
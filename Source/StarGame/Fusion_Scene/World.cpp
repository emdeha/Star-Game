#include "stdafx.h"
#include "World.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_GUI/ControlEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Entities/Scripts.h"
#include "../Fusion_Entities/Spaceship.h"

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

	std::shared_ptr<FusionEngine::CollisionComponent> sunCollisionComponent = std::make_shared<CollisionComponent>();
	sunCollisionComponent->center = glm::vec3();
	sunCollisionComponent->innerRadius = 0.5f;
	sunCollisionComponent->cType = CollisionComponent::FE_COLLISION_CIRCLE;

	sun->AddComponent(FE_COMPONENT_COLLISION, sunCollisionComponent);

	renderer.SubscribeForRendering("sun", sunMesh);
}

void World::CreateEnemy(const std::string &id, glm::vec3 position)
{
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject enemyMesh = meshLoader.LoadAssetObject("mesh-files", "spaceship.obj");

	auto spaceshipRenderComponent = std::make_shared<RenderComponent>();
	spaceshipRenderComponent->vao = enemyMesh.vao;
	spaceshipRenderComponent->shaderProgramID = shaderManager.GetProgram(FE_PROGRAM_LIT).programId;
	spaceshipRenderComponent->renderType = RenderComponent::FE_RENDERER_LIT;
	
	auto spaceshipTransformComponent = std::make_shared<TransformComponent>();
	spaceshipTransformComponent->position = position;
	spaceshipTransformComponent->rotation = glm::vec3();
	spaceshipTransformComponent->scale = glm::vec3(0.1f);

	auto spaceshipCollisionComponent = std::make_shared<CollisionComponent>();
	spaceshipCollisionComponent->center = position;
	spaceshipCollisionComponent->innerRadius = 0.0f;
	spaceshipCollisionComponent->cType = CollisionComponent::FE_COLLISION_CIRCLE;

	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // TODO: Make relative to the Sun

	auto spaceshipEnemyGenericComponent = std::make_shared<EnemyGenericComponent>();
	spaceshipEnemyGenericComponent->health = 100;
	spaceshipEnemyGenericComponent->currentState = FE_STATE_IDLE;
	spaceshipEnemyGenericComponent->speed = 0.02f;
	spaceshipEnemyGenericComponent->frontVector = frontVector;
	spaceshipEnemyGenericComponent->attackRange = 2.0f;

	// Projectile creation
	std::string projectileID = "proj" + id;
	auto spaceshipProjectileComponent = std::make_shared<EnemyProjectileComponent>(projectileID);
	spaceshipProjectileComponent->frontVector = spaceshipEnemyGenericComponent->frontVector;
	spaceshipProjectileComponent->speed = 6 * spaceshipEnemyGenericComponent->speed;
	spaceshipProjectileComponent->range = 3.0f;

	auto projectileRenderableComponent = std::make_shared<RenderComponent>();
	FusionEngine::MeshAssetObject projectileMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");
	projectileRenderableComponent->vao = projectileMesh.vao;
	projectileRenderableComponent->shaderProgramID = shaderManager.GetProgram(FE_PROGRAM_LIT).programId;
	projectileRenderableComponent->renderType = RenderComponent::FE_RENDERER_LIT;

	spaceshipProjectileComponent->components.AddComponent(FE_COMPONENT_RENDER, projectileRenderableComponent);

	auto projectileCollisionComponent = std::make_shared<CollisionComponent>();
	projectileCollisionComponent->center = spaceshipTransformComponent->position;
	projectileCollisionComponent->cType = CollisionComponent::FE_COLLISION_CIRCLE;
	projectileCollisionComponent->innerRadius = 0.05f;

	spaceshipProjectileComponent->components.AddComponent(FE_COMPONENT_COLLISION, projectileCollisionComponent);

	auto projectileTransformComponent = std::make_shared<TransformComponent>();
	projectileTransformComponent->position = spaceshipTransformComponent->position;
	projectileTransformComponent->rotation = glm::vec3();
	projectileTransformComponent->scale = glm::vec3(0.05f);

	spaceshipProjectileComponent->components.AddComponent(FE_COMPONENT_TRANSFORM, projectileTransformComponent);
	// End Projectile creation

	auto firstEnemy = std::make_shared<Enemy>(id);
	firstEnemy->AddComponent(FE_COMPONENT_RENDER, spaceshipRenderComponent);
	firstEnemy->AddComponent(FE_COMPONENT_TRANSFORM, spaceshipTransformComponent);
	firstEnemy->AddComponent(FE_COMPONENT_COLLISION, spaceshipCollisionComponent);
	firstEnemy->AddComponent(FE_COMPONENT_ENEMY_GENERIC, spaceshipEnemyGenericComponent);
	firstEnemy->AddComponent(FE_COMPONENT_ENEMY_PROJECTILE, spaceshipProjectileComponent);
	firstEnemy->SetOnUpdateAI(Spaceship_OnUpdate);
	enemies.push_back(firstEnemy);

	renderer.SubscribeForRendering(id, enemyMesh);
	renderer.SubscribeForRendering(projectileID, projectileMesh);
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

	// Load entities
	FusionEngine::AssetLoader<FusionEngine::EntityAssetObject> entityLoader;
	entityLoader.RegisterType("loader-files", new FusionEngine::EntityLoader());
	loadedEntitiesData = entityLoader.LoadAssetObject("loader-files", "entity-config.yaml");
	auto loadedSkills = loadedEntitiesData.GetLoadedSkills();
	for (auto skill = loadedSkills.begin(); skill != loadedSkills.end(); ++skill)
	{
		if ((*skill).first.find("sat") == std::string::npos)
		{
			sun->AddSkill((*skill).first, (*skill).second);
		}
	}

	// Create enemies
	CreateEnemy("enemysp2", glm::vec3(0.0f, 0.0f, 6.0f));
	CreateEnemy("enemysp1", glm::vec3(4.0f, 0.0f, 0.0f));
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
			EnemyProjectileComponent *enemyProjectile = static_cast<EnemyProjectileComponent*>(
				(*enemy)->GetComponent(FE_COMPONENT_ENEMY_PROJECTILE).get());
			if (enemyProjectile)
			{
				if (enemyProjectile->components.GetID() == id)
				{
					return enemyProjectile->components.GetComponent(componentID);
				}
			}
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "Object with id: " << id << " not found";
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}

std::vector<std::shared_ptr<Composable>> World::GetObjectsWithComponent(ComponentType componentID)
{
	std::vector<std::shared_ptr<Composable>> objectsToReturn;
	if (sun->GetComponent(componentID))
	{
		objectsToReturn.push_back(sun);
	}

	auto sunSats = sun->GetSatellites();
	for (auto sat = sunSats.begin(); sat != sunSats.end(); ++sat)
	{
		if ((*sat)->GetComponent(componentID))
		{
			objectsToReturn.push_back(*sat);
		}
	}

	auto allSkills = sun->GetAllSkills();
	for (auto skill = allSkills.begin(); skill != allSkills.end(); ++skill)
	{
		if ((*skill)->GetComponent(componentID))
		{
			SkillGenericComponent *skillGeneric = static_cast<SkillGenericComponent*>(
				(*skill)->GetComponent(FE_COMPONENT_SKILL_GENERIC).get());
			if (skillGeneric->isActive)
			{
				objectsToReturn.push_back(*skill);
			}
		}
	}

	for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
	{
		if ((*enemy)->GetComponent(componentID))
		{
			objectsToReturn.push_back(*enemy);
		}
		EnemyProjectileComponent *enemyProjectile = static_cast<EnemyProjectileComponent*>(
			(*enemy)->GetComponent(FE_COMPONENT_ENEMY_PROJECTILE).get());
		if (enemyProjectile && enemyProjectile->components.GetComponent(componentID))
		{
			objectsToReturn.push_back(std::make_shared<Composable>(enemyProjectile->components));
		}
	}

	return objectsToReturn;
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
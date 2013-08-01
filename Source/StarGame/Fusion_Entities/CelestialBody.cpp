//Copyright 2012, 2013 Tsvetan Tsvetanov
//This file is part of the Star Game.
//
//The Star Game is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//The Star Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with the Star Game.  If not, see <http://www.gnu.org/licenses/>.


#include "stdafx.h"
#include "CelestialBody.h"

#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/FusionSystems.h"
#include "../Fusion_Scene/Scene.h"


bool AddSatellite(NewCelestialBody *celestialBody,
				  GLuint shaderProg, 
				  float newDiameter, float newOffsetFromSun, float cycleDuration)
{
	// Load mesh
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());
	FusionEngine::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");
	
	FusionEngine::Render *satRender = new FusionEngine::Render();
	
	std::vector<std::shared_ptr<FusionEngine::MeshEntry>> meshEntries = loadedMesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
	{
		satRender->mesh.AddEntry((*meshEntry));
	}
	std::vector<std::shared_ptr<Texture2D>> textures = loadedMesh.GetTextures();
	for(auto texture = textures.begin(); texture != textures.end(); ++texture)
	{
		satRender->mesh.AddTexture((*texture));
	}
	satRender->rendererType = FusionEngine::Render::FE_RENDERER_LIT;
	satRender->shaderProgram = shaderProg;
	satRender->vao = loadedMesh.vao;

	celestialBody->scene.AddEntity("satellite");
	FusionEngine::FunctionalSystem<NewCelestialBody> *satFunctional = 
		new FusionEngine::FunctionalSystem<NewCelestialBody>(celestialBody->scene.GetEventManager(), 
										   celestialBody->scene.GetEntityManager());
	celestialBody->scene.AddSystem(satFunctional);
	celestialBody->scene.AddComponent("satellite", satRender);

	FusionEngine::Transform *satTransform = new FusionEngine::Transform();
	satTransform->position = glm::vec3();
	satTransform->rotation = glm::vec3();
	satTransform->scale = glm::vec3(celestialBody->diameter);
	celestialBody->scene.AddComponent("satellite", satTransform);

	FusionEngine::Functional<NewCelestialBody> *satFuncComp = new FusionEngine::Functional<NewCelestialBody>();
	satFuncComp->UpdateFunction = Update;
	celestialBody->scene.AddComponent("satellite", satFuncComp);


	std::shared_ptr<NewCelestialBody> newSat(new NewCelestialBody(celestialBody->scene, 1.0f, 2.0f, 5.0f));
	celestialBody->satellites.push_back(newSat);
	//newSat->parent = celestialBody;


	return true;
}

void Update(NewCelestialBody *celestialBody)
{
	FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
		celestialBody->scene.GetEntityManager()->GetComponentList(celestialBody->scene.GetEntity("sun"), FusionEngine::CT_TRANSFORM);

	transformData[0]->position.x += 0.001f;
	

	/*
	for(auto satellite = celestialBody->satellites.begin(); satellite != celestialBody->satellites.end(); ++satellite)
	{
		// TODO: Get entity by id
		FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
			celestialBody->scene.GetEntityManager()->GetComponentList(celestialBody->scene.GetEntity("satellite"), FusionEngine::CT_TRANSFORM);

		(*satellite)->revolutionTimer.Update();

		float currentTimeThroughLoop = (*satellite)->revolutionTimer.GetAlpha();
		
		transformData[0]->position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
		transformData[0]->position.y = cosf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
	}
	*/
}
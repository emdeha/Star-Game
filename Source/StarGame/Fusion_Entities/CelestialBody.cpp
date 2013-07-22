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


bool CelestialBody::AddSatellite(FusionEngine::Scene &scene, GLuint shaderProg)
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

	scene.AddEntity("satellite");
	FusionEngine::FunctionalSystem *satFunctional = 
		new FusionEngine::FunctionalSystem(scene.GetEventManager(), scene.GetEntityManager());
	scene.AddSystem(satFunctional);
	scene.AddComponent("satellite", satRender);

	FusionEngine::Transform *satTransform = new FusionEngine::Transform();
	satTransform->position = glm::vec3();
	satTransform->rotation = glm::vec3();
	satTransform->scale = glm::vec3(diameter);
	scene.AddComponent("satellite", satTransform);

	FusionEngine::Functional *satFuncComp = new FusionEngine::Functional();
	satFuncComp->UpdateFunction = this->Update; //  add scene as a param to the func ptr
	scene.AddComponent("satellite", satFuncComp);


	std::shared_ptr<CelestialBody> newSat(new CelestialBody(1.0f, 2.0f, 5.0f));
	satellites.push_back(newSat);

	return true;
}

void CelestialBody::Update(FusionEngine::Scene &scene)
{
	for(auto satellite = satellites.begin(); satellite != satellites.end(); ++satellite)
	{
		// TODO: Get entity by id
		FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
			scene.GetEntityManager()->GetComponentList(scene.GetEntity("satellite"), FusionEngine::CT_TRANSFORM);

		(*satellite)->revolutionTimer.Update();

		float currentTimeThroughLoop = (*satellite)->revolutionTimer.GetAlpha();
		
		transformData[0]->position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
		transformData[0]->position.y = cosf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	}
}
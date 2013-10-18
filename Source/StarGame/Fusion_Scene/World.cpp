#include "stdafx.h"
#include "World.h"


using namespace FusionEngine;

World::~World()
{
}

void World::Render(glutil::MatrixStack &modelMatrix) 
{
	sunLight.Render(modelMatrix, shaderManager.GetLitProgData(), shaderManager.GetUniformBuffer(UBT_LIGHT));
	renderer.Render(modelMatrix);
}
#include "stdafx.h"
#include "World.h"


using namespace FusionEngine;

World::~World()
{
}

void World::Render(glutil::MatrixStack &modelMatrix) const
{
	renderer.Render(modelMatrix);
}
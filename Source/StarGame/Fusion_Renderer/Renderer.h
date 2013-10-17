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


#ifndef RENDERER_H
#define RENDERER_H


#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include <vector>

#include "../Fusion_EntitySystem/Component.h"
#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionSystems.h"
#include "../Fusion_EntitySystem/Entity.h"
#include "../Fusion_AssetLoader/AssetLoader.h"


namespace FusionEngine
{
	class Renderer
	{
	private:
		std::vector<std::pair<unsigned int, MeshAssetObject>> subscribedMeshes;		

	public:
        Renderer() {}

		void SubscribeForRendering(Entity *entity);
		void UnsubscribeForRendering(Entity *entity);

		void Render(glutil::MatrixStack &modelMatrix) const;
	};
}


#endif
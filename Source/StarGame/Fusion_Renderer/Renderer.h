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


#ifndef FE_RENDERER_H
#define FE_RENDERER_H


#pragma warning(push, 0)
#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include <vector>
#include <map>
#pragma warning(pop)

#include "../Fusion_EntitySystem/Entity.h"
#include "../Fusion_AssetLoader/AssetLoader.h"


namespace FusionEngine
{
	class Renderer
	{
	private:
		std::vector<std::pair<unsigned int, MeshAssetObject>> subscribedMeshes;		

		std::map<unsigned int, GLuint> modelToCameraMatrixUniform;
		std::map<unsigned int, GLuint> colorUniform;
		std::map<unsigned int, GLuint> normalModelToCameraMatrixUniform;
		
		std::map<unsigned int, GLuint> positionAttrib;
		std::map<unsigned int, GLuint> textureAttrib;
		std::map<unsigned int, GLuint> normalAttrib;

	public:
        Renderer() {}

		void SubscribeForRendering(Entity *entity);
		void UnsubscribeForRendering(Entity *entity);

		void Render(glutil::MatrixStack &modelMatrix) const;
	};
}


#endif
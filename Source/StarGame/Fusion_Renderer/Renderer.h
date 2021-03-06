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


#pragma warning(push, 1)
#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include <map>
#pragma warning(pop)

#include "../Fusion_AssetLoader/AssetLoader.h"


namespace FusionEngine
{
	class Renderer
	{
	private:
		std::map<std::string, MeshAssetObject> subscribedMeshes;		

		std::map<std::string, GLuint> modelToCameraMatrixUniform;
		std::map<std::string, GLuint> colorUniform;
		std::map<std::string, GLuint> normalModelToCameraMatrixUniform;
		
		std::map<std::string, GLuint> positionAttrib;
		std::map<std::string, GLuint> textureAttrib;
		std::map<std::string, GLuint> normalAttrib;

	public:
        Renderer() {}

		void SubscribeForRendering(const std::string &id, const MeshAssetObject &mesh);
		void UnsubscribeForRendering(const std::string &id);

		void Render(glutil::MatrixStack &modelMatrix) const;
	};
}


#endif
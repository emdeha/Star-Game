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


#ifndef SHADER_ENUMS_H
#define SHADER_ENUMS_H


namespace FusionEngine
{
	enum ProgramType
	{
		FE_LIT_PROGRAM,
		FE_LIT_TEXTURE_PROGRAM,
		FE_UNLIT_PROGRAM,
		FE_SIMPLE_PROGRAM,
		FE_SIMPLE_NO_UB_PROGRAM,
		FE_FONT_PROGRAM,
		FE_SIMPLE_TEXTURE_PROGRAM,
		FE_TEXTURE_PROGRAM,
		FE_TEXTURE_PERSPECTIVE_PROGRAM,
		FE_BILLBOARD_NO_TEXTURE_PROGRAM,
		FE_BILLBOARD_PROGRAM,
		FE_PARTICLE_PROGRAM,
		FE_SPRITE_PARTICLE_PROGRAM,
	};

	enum BlockType
	{
		BT_MATERIAL,
		BT_LIGHT, 
		BT_PROJECTION,
		BT_ORTHOGRAPHIC,
	};

	enum UniformBufferType
	{
		UBT_PROJECTION,
		UBT_ORTHOGRAPHIC, 
		UBT_LIGHT,
	};
}

#endif
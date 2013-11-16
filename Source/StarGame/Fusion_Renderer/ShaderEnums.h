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
		FE_PROGRAM_BAD = -1,

		FE_PROGRAM_LIT = 0,
		FE_PROGRAM_LIT_TEXTURE,
		FE_PROGRAM_UNLIT,
		FE_PROGRAM_SIMPLE,
		FE_PROGRAM_SIMPLE_NO_UB,
		FE_PROGRAM_FONT,
		FE_PROGRAM_SIMPLE_TEXTURE,
		FE_PROGRAM_TEXTURE,
		FE_PROGRAM_TEXTURE_PERSPECTIVE,
		FE_PROGRAM_BILLBOARD_NO_TEXTURE,
		FE_PROGRAM_BILLBOARD,
		FE_PROGRAM_PARTICLE,
		FE_PROGRAM_SPRITE_PARTICLE,
	};

	// TODO: Generate from file?
	enum UniformType
	{
		FE_UNIFORM_BAD = -1,

		FE_UNIFORM_MODEL_TO_CAMERA_MATRIX = 0,
		FE_UNIFORM_NORMAL_MODEL_TO_CAMERA_MATRIX,
		FE_UNIFORM_COLOR_TEXTURE,
		FE_UNIFORM_COLOR,
		FE_UNIFORM_OBJECT_COLOR,
		FE_UNIFORM_CAMERA_TO_CLIP_MATRIX,
		FE_UNIFORM_FONT_COLOR,
		FE_UNIFORM_FONT_TEXTURE,
		FE_UNIFORM_PROJECTION,
		FE_UNIFORM__SAMPLER,
		FE_UNIFORM_CAMERA_POSITION,
		FE_UNIFORM_DELTA_TIME_MILLISECONDS,
		FE_UNIFORM_LAUNCHER_LIFETIME,
		FE_UNIFORM_RANDOM_DIRECTIONS,
		FE_UNIFORM_SECONDARY_SHELL_LIFETIME,
		FE_UNIFORM_SHELL_LIFETIME,
		FE_UNIFORM_TIME,
		FE_UNIFORM_DELTA_POSITION,
		FE_UNIFORM_BILLBOARD_SIZE,
		FE_UNIFORM_LIGHT_INTENSITY,
		FE_UNIFORM_CAMERA_SPACE_LIGHT_POS,
	};

	enum AttribType
	{
		FE_ATTRIB_BAD = -1,

		FE_ATTRIB_POSITION = 0,
		FE_ATTRIB_NORMAL,
		FE_ATTRIB_TEX_COORD,
		FE_ATTRIB_AGE,
		FE_ATTRIB_TYPE,
		FE_ATTRIB_VELOCITY,
	};
	
	enum BlockType
	{
		FE_BT_BAD = -1,

		FE_BT_MATERIAL = 0,
		FE_BT_LIGHT, 
		FE_BT_PROJECTION,
		FE_BT_ORTHOGRAPHIC,
	};

	enum UniformBufferType
	{
		FE_UBT_BAD = -1,

		FE_UBT_PROJECTION = 0,
		FE_UBT_ORTHOGRAPHIC, 
		FE_UBT_LIGHT,
	};
}

#endif
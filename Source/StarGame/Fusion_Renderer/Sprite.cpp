#include "stdafx.h"
#include "Sprite.h"


#include "../framework/ErrorAPI.h"
#include "../Fusion_Scene/World.h"


using namespace FusionEngine;


void Sprite2D::Init(const std::string &textureFileName)
{
	std::vector<float> vertexData;
	std::vector<float> texCoordsData;
	std::vector<unsigned short> indexData;

	vertexData.push_back(position.x);
	vertexData.push_back(position.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);


	texCoordsData.push_back(0.0f); texCoordsData.push_back(1.0f);
	texCoordsData.push_back(1.0f); texCoordsData.push_back(1.0f);
	texCoordsData.push_back(1.0f); texCoordsData.push_back(0.0f);
	texCoordsData.push_back(0.0f); texCoordsData.push_back(0.0f);


	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);	
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &texCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * texCoordsData.size(), &texCoordsData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

	if(!texture->Load(textureFileName, GL_RGB, GL_BGR, GL_UNSIGNED_BYTE, false))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}

	ProgramData textureData = GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_TEXTURE);
	programId = textureData.programId;
	modelToCameraMatrixUnif = textureData.GetUniform(FE_UNIFORM_MODEL_TO_CAMERA_MATRIX);
	colorUnif = textureData.GetUniform(FE_UNIFORM_COLOR);
	positionAttrib = textureData.GetAttrib(FE_ATTRIB_POSITION);
	texCoordAttrib = textureData.GetAttrib(FE_ATTRIB_TEX_COORD);
}

void Sprite2D::SetPosition(glm::vec2 newPosition)
{
	position = newPosition;

	std::vector<float> vertexData;

	vertexData.push_back(position.x);
	vertexData.push_back(position.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertexData.size(), &vertexData[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite2D::SetDimensions(float newWidth, float newHeight)
{
	width = newWidth;
	height = newHeight;

	SetPosition(position);
}

void Sprite2D::Draw(glutil::MatrixStack &modelMatrix) const
{
	glUseProgram(programId);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(colorUnif, 1.0f, 1.0f, 1.0f, 1.0f);

		glEnableVertexAttribArray(positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(texCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordsBO);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		texture->Bind(GL_TEXTURE0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);


		glDisableVertexAttribArray(positionAttrib);
		glDisableVertexAttribArray(texCoordAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

float Sprite2D::GetWidth() const
{
	return width;
}
float Sprite2D::GetHeight() const
{
	return height;
}
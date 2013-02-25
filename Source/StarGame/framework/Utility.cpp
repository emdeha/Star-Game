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


#include "Utility.h"
#include "../framework/ErrorAPI.h"
#include <sstream>

#define PI 3.14159f


glm::vec4 Utility::CorrectGamma(const glm::vec4 &inputColor, float gamma)
{
	glm::vec4 outputColor;
	outputColor[0] = powf(inputColor[0], 1.0f / gamma);
	outputColor[1] = powf(inputColor[1], 1.0f / gamma);
	outputColor[2] = powf(inputColor[2], 1.0f / gamma);
	outputColor[3] = inputColor[3];

	return outputColor;
}


void Utility::CalculateFPS()
{

}

// iterative would make performance gain
int Utility::GetFibonacciNumber(int whichNumber)
{
	if(whichNumber < 2)
	{
		return 1;
	}
	else
	{
		return GetFibonacciNumber(whichNumber - 1) + GetFibonacciNumber(whichNumber - 2);
	}
}

std::vector<std::string> Utility::SplitString(const std::string &str, char delim)
{
	std::vector<std::string> splittedElems;
	std::stringstream ss(str);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		splittedElems.push_back(item);
	}
	return splittedElems;
}


Utility::Ray::Ray()
{
	origin = glm::vec4();
	direction = glm::vec4();
}
Utility::Ray::Ray(glm::vec4 newOrigin, glm::vec4 newDirection)
{
	origin = newOrigin;
	direction = newDirection;
}


bool Utility::ClickDetection::IsTorusClicked(float outerRadius, float innerRadius, 
											 glm::vec3 position,/* glm::vec4 cameraPosition,
											 glm::mat4 projectionMatrix, glm::mat4 modelMatrix,
											 int windowWidth, int windowHeight,*/
											 Utility::Ray mouseRay)
{/*
	Utility::Ray mouseRay = userMouse.GetPickRay(projectionMatrix, modelMatrix, cameraPosition,
												 windowWidth, windowHeight);
	*/
	glutil::MatrixStack distortedMatrix;
	distortedMatrix.Scale(1.0f, 1.0f, 5.0f);

	if(Utility::Intersections::RayIntersectsEllipsoid(mouseRay, position, outerRadius, distortedMatrix.Top()) &&
       !Utility::Intersections::RayIntersectsEllipsoid(mouseRay, position, innerRadius, distortedMatrix.Top()))
	{
		return true;
	}
	return false;
}


bool Utility::Intersections::RayIntersectsSphere(Ray mouseRay, glm::vec3 bodyPosition, 
												 float sphereRadius)
{
	glm::vec4 mouseRayOrigin = mouseRay.origin - glm::vec4(bodyPosition, 1.0f);

	float a = glm::dot(mouseRay.direction, mouseRay.direction);
	float b = glm::dot(mouseRay.direction, mouseRayOrigin) * 2.0f;
	float c = glm::dot(mouseRayOrigin, mouseRayOrigin) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4 * a * c;

	if(discriminant >= 0.0f)
	{
		return true;
	}
	else 
		return false;
}
bool Utility::Intersections::RayIntersectsEllipsoid(Ray mouseRay, glm::vec3 bodyPosition, 
													float sphereRadius, glm::mat4 deformationMat)
{
	Ray distortedRay = Ray(deformationMat * mouseRay.origin, deformationMat * mouseRay.direction);
	return Utility::Intersections::RayIntersectsSphere(distortedRay, bodyPosition, sphereRadius);
}


void Utility::Primitives::InitTorus2DData(std::vector<float> &vertexData, 
										  std::vector<unsigned short> &indexData,
										  int resolution, 
										  float outerRadius, float innerRadius,
										  glm::vec3 position)
{
	unsigned short currentIndex = 0;
	int currentIndexPos = 0;

	for(int i = 0; i <= 360; i += 360 / resolution)
	{		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(position.y);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(1.0f);
		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(position.y);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(1.0f);

		indexData.push_back(currentIndex + 1);
		indexData.push_back(currentIndex + 2);		

		currentIndexPos += 2;
		currentIndex += 2;
	}

	indexData[currentIndexPos - 1] = 1;
	indexData.push_back(2);
	indexData.push_back(currentIndex - 2);
}


Utility::Primitives::Torus2D::Torus2D()
{
	resolution = 3;
	
	color = glm::vec4();
	position = glm::vec3();

	innerRadius = 0.0f;
	outerRadius = 0.0f;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
}

Utility::Primitives::Torus2D::Torus2D(glm::vec4 newColor, glm::vec3 newPosition,
									  float newInnerRadius, float newOuterRadius, 
									  int newResolution)
{
	assert(newResolution < 360 && newResolution > 3);

	color = newColor;
	position = newPosition;

	innerRadius = newInnerRadius;
	outerRadius = newOuterRadius;
	resolution = newResolution;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
}


void Utility::Primitives::Torus2D::Init()
{
	// Torus Generation
	std::vector<float> vertexData;
	std::vector<unsigned short> indexData;

	unsigned short currentIndex = 0;
	int currentIndexPos = 0;

	for(int i = 0; i <= 360; i += 360 / resolution)
	{		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(position.y);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(1.0f);
		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(position.y);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(1.0f);

		indexData.push_back(currentIndex + 1);
		indexData.push_back(currentIndex + 2);		

		currentIndexPos += 2;
		currentIndex += 2;
	}

	indexData[currentIndexPos - 1] = 1;
	indexData.push_back(2);
	indexData.push_back(currentIndex - 2);


	// Initialization of buffer objects
	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);

	glBindVertexArray(0);
}

void Utility::Primitives::Torus2D::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &data)
{
	glUseProgram(data.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(data.colorUnif, color.r, color.g, color.b, color.a);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glEnableVertexAttribArray(data.positionAttrib);
		glVertexAttribPointer(data.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLE_STRIP, resolution * 4, GL_UNSIGNED_SHORT, 0);
	}
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}



Utility::Primitives::Circle::Circle(glm::vec4 newColor, glm::vec3 newPosition,
									float newRadius, short newResolution)
{
	assert(newResolution < 360 && newResolution > 3.0);

	color = newColor;
	position = newPosition;

	radius = newRadius;
	resolution = newResolution;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
}

void Utility::Primitives::Circle::Init()
{
	// Circle Generation
	std::vector<float> vertexData;
	std::vector<unsigned short> indexData;

	unsigned short currentIndex = 0;

	for(int i = 0; i <= 360; i += 360 / resolution)
	{		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * radius);
		vertexData.push_back(-0.1f);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * radius);
		vertexData.push_back(1.0f);

		indexData.push_back(currentIndex);
		
		currentIndex++;
	}


	// Initialization of buffer objects
	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);

	glBindVertexArray(0);
}

void Utility::Primitives::Circle::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &data)
{
	glUseProgram(data.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(data.colorUnif, color.r, color.g, color.b, color.a);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glEnableVertexAttribArray(data.positionAttrib);
		glVertexAttribPointer(data.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLE_FAN, resolution * 4, GL_UNSIGNED_SHORT, 0);
	}
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}



Utility::Primitives::Square::Square(glm::vec4 newColor, 
									glm::vec3 newPosition,
									float newWidth, float newHeight,
									bool newIsCoordinateSytemBottomLeft)
{
	color = newColor;
	position = newPosition;
	width = newWidth;
	height = newHeight;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;

	isCoordinateSystemBottomLeft = newIsCoordinateSytemBottomLeft;
}

void Utility::Primitives::Square::Init(int windowWidth, int windowHeight)
{
	glm::vec2 positionRelativeToCoordSystem = glm::vec2(position);
	if(isCoordinateSystemBottomLeft)
	{
		positionRelativeToCoordSystem.x = windowWidth - positionRelativeToCoordSystem.x;
		positionRelativeToCoordSystem.y = windowHeight - positionRelativeToCoordSystem.y;
	}


	std::vector<float> vertexData;
	std::vector<unsigned short> indexData;

	
	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(0.0f); vertexData.push_back(1.0f);


	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);
	

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);

	glBindVertexArray(0);
}

void Utility::Primitives::Square::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &data)
{
	glUseProgram(data.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(data.colorUnif, color.r, color.g, color.b, color.a);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glEnableVertexAttribArray(data.positionAttrib);
		glVertexAttribPointer(data.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}



Utility::Primitives::Sprite::Sprite(glm::vec3 newPosition, glm::vec4 newColor,
									float newWidth, float newHeight,
									bool newIsCoordinateSystemBottomLeft)
{
	color = newColor;
	position = newPosition;
	width = newWidth;
	height = newHeight;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
	textureCoordsBO = 0;

	isCoordinateSystemBottomLeft = newIsCoordinateSystemBottomLeft;

	texture = std::shared_ptr<Texture2D>(new Texture2D());
}

void Utility::Primitives::Sprite::Init(const std::string &textureFileName, 
									   int windowWidth, int windowHeight)
{
	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	
	glm::vec3 positionRelativeToCoordSystem = glm::vec3(position);
	if(isCoordinateSystemBottomLeft)
	{
	}


	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);


	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(0.0f);
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(0.0f);


	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);	
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &textureCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * textureCoordsData.size(), &textureCoordsData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

	if(!texture->Load(textureFileName))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}

void Utility::Primitives::Sprite::Draw(glutil::MatrixStack &modelMat, const TextureProgData &textureData)
{
	glUseProgram(textureData.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(
			textureData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMat.Top()));

		glUniform4f(textureData.colorUnif, color.r, color.g, color.b, color.a);

		glEnableVertexAttribArray(textureData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(textureData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
		
		glEnableVertexAttribArray(textureData.texturePosAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(textureData.texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);


		texture->Bind(GL_TEXTURE0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);


		glDisableVertexAttribArray(textureData.positionAttrib);
		glDisableVertexAttribArray(textureData.texturePosAttrib);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void Utility::Primitives::Sprite::ChangeTexture(const std::string &textureFileName)
{
	texture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!texture->Load(textureFileName))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}


Utility::Primitives::Sprite3D::Sprite3D(glm::vec3 newPosition, float newWidth, float newHeight)
{
	position = newPosition;
	height = newHeight;
	width = newWidth;
	
	vao = 0;
	indexBO = 0;
	vertexBO = 0;
	textureCoordsBO = 0;

	texture = std::shared_ptr<Texture2D>(new Texture2D());
}

void Utility::Primitives::Sprite3D::Init(const std::string &textureFileName)
{
	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - height); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - height); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - width);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);
		
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(0.0f);
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(0.0f);

	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);	
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &textureCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * textureCoordsData.size(), &textureCoordsData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// TODO: Better error-handling needed
	if(!texture->Load(textureFileName))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}

void Utility::Primitives::Sprite3D::Draw(glutil::MatrixStack &modelMat, const SimpleTextureProgData &textureData)
{
	glUseProgram(textureData.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(
			textureData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMat.Top()));
		
		glEnableVertexAttribArray(textureData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(textureData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(textureData.textureCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(textureData.textureCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glUniform4f(textureData.colorUnif, 0.0f, 0.0f, 0.0f, 1.0f);

		texture->Bind(GL_TEXTURE0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(textureData.positionAttrib);
		glDisableVertexAttribArray(textureData.textureCoordAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		texture->Unbind();
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void Utility::Primitives::Sprite3D::ChangeTexture(const std::string &textureFileName)
{
	texture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!texture->Load(textureFileName))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}

glm::vec3 Utility::Primitives::Sprite3D::GetPosition()
{
	return position;
}


Utility::Primitives::SpriteArray::SpriteArray(glm::vec4 newColor,
										  	  float newWidth, float newHeight,
											  bool newIsCoordinateSystemBottomLeft)
{
	color = newColor;
	positions.resize(0);
	width = newWidth;
	height = newHeight;

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
	textureCoordsBO = 0;

	isCoordinateSystemBottomLeft = newIsCoordinateSystemBottomLeft;

	texture = std::shared_ptr<Texture2D>(new Texture2D());
}

void Utility::Primitives::SpriteArray::Init(int newSpritesCount,
											const std::string &textureFileName, 
											int windowWidth, int windowHeight)
{
	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	spritesCount = newSpritesCount;
	

	glm::vec3 positionRelativeToCoordSystem = glm::vec3();
	if(isCoordinateSystemBottomLeft)
	{
	}


	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y - height);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x - width);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	vertexData.push_back(positionRelativeToCoordSystem.x);
	vertexData.push_back(positionRelativeToCoordSystem.y);
	vertexData.push_back(positionRelativeToCoordSystem.z); vertexData.push_back(1.0f);

	
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(0.0f);
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(0.0f);


	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &textureCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * textureCoordsData.size(), &textureCoordsData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if(!texture->Load(textureFileName))
	{
		std::string errorMessage = "cannot load texture ";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}
/*
void Utility::Primitives::SpriteArray::AddPosition(glm::vec4 newPosition)
{
	positions.push_back(newPosition);

	
	glm::vec3 positionRelativeToCoordSystem = glm::vec3(positions.back());
	if(isCoordinateSystemBottomLeft)
	{
	}


	std::vector<float> updatedVertexData;

	updatedVertexData.push_back(positionRelativeToCoordSystem.x);
	updatedVertexData.push_back(positionRelativeToCoordSystem.y - height);
	updatedVertexData.push_back(positionRelativeToCoordSystem.z); updatedVertexData.push_back(1.0f);

	updatedVertexData.push_back(positionRelativeToCoordSystem.x - width);
	updatedVertexData.push_back(positionRelativeToCoordSystem.y - height);
	updatedVertexData.push_back(positionRelativeToCoordSystem.z); updatedVertexData.push_back(1.0f);

	updatedVertexData.push_back(positionRelativeToCoordSystem.x - width);
	updatedVertexData.push_back(positionRelativeToCoordSystem.y);
	updatedVertexData.push_back(positionRelativeToCoordSystem.z); updatedVertexData.push_back(1.0f);

	updatedVertexData.push_back(positionRelativeToCoordSystem.x);
	updatedVertexData.push_back(positionRelativeToCoordSystem.y);
	updatedVertexData.push_back(positionRelativeToCoordSystem.z); updatedVertexData.push_back(1.0f);


	int offset = (positions.size() - 1) * 16;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float) * updatedVertexData.size(), &updatedVertexData[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}*/

void Utility::Primitives::SpriteArray::Draw(glutil::MatrixStack &modelMat,
											const TextureProgData &textureData,
											std::vector<glm::vec3> particlePositions)
{
	glUseProgram(textureData.theProgram);
	glBindVertexArray(vao);
	{
		glUniform4f(textureData.colorUnif, color.r, color.g, color.b, color.a);

		glEnableVertexAttribArray(textureData.texturePosAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(textureData.texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		

		texture->Bind(GL_TEXTURE0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);

		for(int i = 0; i < spritesCount; i++)
		{
			glutil::PushStack push(modelMat);

			modelMat.Translate(particlePositions[i].x, particlePositions[i].y, particlePositions[i].z);
			
			glEnableVertexAttribArray(textureData.positionAttrib);
			glVertexAttribPointer(textureData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

			glUniformMatrix4fv(
				textureData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMat.Top()));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}


		glDisableVertexAttribArray(textureData.positionAttrib);
		glDisableVertexAttribArray(textureData.texturePosAttrib);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}
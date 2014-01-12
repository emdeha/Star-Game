#include "stdafx.h"
#include "Light.h"

SunLight::SunLight()
{
    position = glm::vec3();
    lightIntensity = glm::vec4();
    ambientIntensity = glm::vec4();
    lightAttenuation = 0.0f;
    maxIntensity = 0.0f;
    gamma = 2.2f;
}
SunLight::SunLight(glm::vec3 newPosition,
                   glm::vec4 newLightIntensity, glm::vec4 newAmbientIntensity,
                   float newLightAttenuation, 
                   float newMaxIntensity, float newGamma)
{
    position = newPosition;
    lightIntensity = newLightIntensity;
    ambientIntensity = newAmbientIntensity;
    lightAttenuation = newLightAttenuation;
    maxIntensity = newMaxIntensity;
    gamma = newGamma;
}

void SunLight::Render(glutil::MatrixStack &modelMatrix, FusionEngine::ShaderManager shaderManager)
{
    glm::vec4 position_cameraSpace = modelMatrix.Top() * glm::vec4(position, 1.0f);

	FusionEngine::ProgramData lightData =
		shaderManager.GetProgram(FusionEngine::FE_PROGRAM_LIT);
    glUseProgram(lightData.programId);

    glUniform4fv(lightData.GetUniform(FusionEngine::FE_UNIFORM_LIGHT_INTENSITY), 1, glm::value_ptr(lightIntensity));
    glUniform3fv(lightData.GetUniform(FusionEngine::FE_UNIFORM_CAMERA_SPACE_LIGHT_POS),
			 	 1, glm::value_ptr(position_cameraSpace));

    
    LightBlockGamma blockLightData;

    blockLightData.ambientIntensity = ambientIntensity;
    blockLightData.lightAttenuation = lightAttenuation;
    blockLightData.maxIntensity = maxIntensity;
    blockLightData.gamma = gamma;
	
	
	glBindBuffer(GL_UNIFORM_BUFFER, shaderManager.GetUniformBuffer(FusionEngine::FE_UBT_LIGHT));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(blockLightData), &blockLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


	FusionEngine::ProgramData litTextureData =
		shaderManager.GetProgram(FusionEngine::FE_PROGRAM_LIT_TEXTURE);
	glUseProgram(litTextureData.programId);

    glUniform4fv(litTextureData.GetUniform(FusionEngine::FE_UNIFORM_LIGHT_INTENSITY), 1, glm::value_ptr(lightIntensity));
    glUniform3fv(litTextureData.GetUniform(FusionEngine::FE_UNIFORM_CAMERA_SPACE_LIGHT_POS),
				 1, glm::value_ptr(position_cameraSpace));

    blockLightData.ambientIntensity = ambientIntensity;
    blockLightData.lightAttenuation = lightAttenuation;
    blockLightData.maxIntensity = maxIntensity;
    blockLightData.gamma = gamma;


    glBindBuffer(GL_UNIFORM_BUFFER, shaderManager.GetUniformBuffer(FusionEngine::FE_UBT_LIGHT));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(blockLightData), &blockLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUseProgram(0);
}
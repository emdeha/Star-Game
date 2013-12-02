#include "stdafx.h"
#include "Control.h"

#include "../Fusion_EntitySystem/EntityEvents.h"


using namespace FusionEngine;


void Control::SetRelativity(RelativityOption relativeTo)
{
	switch (relativeTo)
	{
	case FE_RELATIVE_BOTTOM_LEFT:
		position = glm::ivec2(windowWidth - initialPosition.x, windowHeight - initialPosition.y);
		break;
	case FE_RELATIVE_BOTTOM_RIGHT:
		position = glm::ivec2(initialPosition.x, windowHeight - initialPosition.y);
		break;
	case FE_RELATIVE_TOP_LEFT:
		position = glm::ivec2(windowWidth - initialPosition.x, initialPosition.y);	
		break;
	case FE_RELATIVE_TOP_RIGHT:
		position = initialPosition;
		break;
	case FE_RELATIVE_CENTER_BOTTOM:
		position = glm::ivec2((windowWidth / 2) - initialPosition.x, windowHeight - initialPosition.y);
		break;
	case FE_RELATIVE_CENTER_TOP:
		position = glm::ivec2((windowWidth / 2) - initialPosition.x, initialPosition.y);
		break;
	}

	currentRelativity = relativeTo;
}

void Control::Init(const std::string &backgroundImageFileName,
				   EventManager &eventManager)
{
	//glm::vec2 backgorundPos = 
	//	glm::vec2(windowWidth - position.x, windowHeight - position.y);
	background = Sprite2D(glm::vec2(position), width, height);
	background.Init(backgroundImageFileName);

	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

void Control::Draw(glutil::MatrixStack &modelMatrix)
{
	glutil::MatrixStack identityMatrix;
	identityMatrix.SetIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	background.Draw(identityMatrix);

	glDisable(GL_BLEND);
}

bool Control::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case EVENT_ON_RESHAPE:
		{
			const OnReshapeEvent &data = static_cast<const OnReshapeEvent&>(eventData);
			windowHeight = data.windowHeight;
			windowWidth = data.windowWidth;

			SetRelativity(currentRelativity);
			background = Sprite2D(glm::vec2(position), width, height);
			background.Init("../data/images/b-middle-section.jpg");
		}
		break;
	}
	
	return false;
}
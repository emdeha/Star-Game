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

void Control::SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
								glm::vec4 newTextColor, int newTextSize)
{
	textFont = newTextFont;
	textString = newTextString;
	textColor = newTextColor;
	textSize = newTextSize;
	textPosition = glm::vec2(position.x - margins.x,
							 position.y - margins.y);
}

void Control::Init(const std::string &backgroundImageFileName,
				   EventManager &eventManager)
{
	background = Sprite2D(glm::vec2(position), width, height);
	background.Init(backgroundImageFileName);

	if (textString == "" || textFont == ""  || textSize == -1 || 
		textColor == glm::vec4(-1.0f) || textPosition == glm::vec2(-1.0f))
	{
		std::string errorMessage = "one or more of the text\'s properties are not initialized ";
		errorMessage += "Control: " + textString; // TODO: textString should be controlName
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
	text = Text(textFont, textString, textPosition, textColor, textSize);
	text.Init(windowWidth, windowHeight); 

	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

void Control::Draw(glutil::MatrixStack &modelMatrix)
{
	glutil::MatrixStack identityMatrix;
	identityMatrix.SetIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	background.Draw(identityMatrix);
	text.Draw();

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
			background.SetPosition(glm::vec2(position));
			textPosition = glm::vec2(position.x - margins.x,
									 position.y - margins.y);
			text.SetPosition(textPosition, windowWidth, windowHeight);
		}
		break;
	}
	
	return false;
}
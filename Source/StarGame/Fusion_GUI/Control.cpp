#include "stdafx.h"
#include "Control.h"

#include "../Fusion_EntitySystem/EntityEvents.h"


using namespace FusionEngine;


void Control::SetOnKeyPressedHandler(EventHandlingFunc onKeyPressedHandler)
{
	handleOnKeyPress = onKeyPressedHandler;
}

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

void Control::SetVisibility(bool newIsVisible)
{
	isVisible = newIsVisible;
}
bool Control::IsVisible() const
{
	return isVisible;
}

void Control::SetActive(bool newIsActive)
{
	isActive = newIsActive;
}
bool Control::IsActive() const
{
	return isActive;
}

void Control::SetBackground(const std::string &backgroundFileName)
{
	background = Sprite2D(glm::vec2(position), width, height);
	background.Init(backgroundFileName);

	hasBackground = true;
}

void Control::Init(EventManager &eventManager)
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

void Control::Draw(glutil::MatrixStack &modelMatrix) const
{
	if (isVisible && hasBackground)
	{
		glutil::MatrixStack identityMatrix;
		identityMatrix.SetIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		background.Draw(identityMatrix);

		glDisable(GL_BLEND);
	}
}

bool Control::IsMouseOn(glm::ivec2 mouseCoordinates_windowSpace) const
{
	mouseCoordinates_windowSpace.x = windowWidth - mouseCoordinates_windowSpace.x;

	glm::ivec2 minCorner = position - glm::ivec2(width, height);
	glm::ivec2 maxCorner = position;

	if(mouseCoordinates_windowSpace.y > minCorner.y &&
	   mouseCoordinates_windowSpace.y < maxCorner.y &&
	   mouseCoordinates_windowSpace.x > minCorner.x &&
	   mouseCoordinates_windowSpace.x < maxCorner.x)
	{
		return true;
	}
	else return false;
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
			if (hasBackground)
			{
				background.SetPosition(glm::vec2(position));
			}
		}
		break;
	}
	
	return false;
}

std::string Control::GetName() const
{
	return name;
}


///////////////////
//  TextControl  //
///////////////////
void TextControl::SetOnClickHandler(EventHandlingFunc onClickHandler)
{
	handleOnClick = onClickHandler;
}
void TextControl::SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
									glm::vec4 newTextColor, unsigned short newTextSize)
{
	textFont = newTextFont;
	textString = newTextString;
	textColor = newTextColor;
	textSize = newTextSize;
	textPosition = glm::vec2(position.x - margins.x,
							 position.y - margins.y);
}

std::string TextControl::GetText() const
{
	return textString;
}

void TextControl::Init(EventManager &eventManager)
{
	Control::Init(eventManager);

	if (textFont == ""  || textSize == -1 || 
		textColor == glm::vec4(-1.0f) || textPosition == glm::vec2(-1.0f))
	{
		std::string errorMessage = "one or more of the text\'s properties are not initialized ";
		errorMessage += "Control: ==* " + name + " *==";
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
	text = Text(textFont, textString, textPosition, textColor, textSize);
	text.Init(windowWidth, windowHeight); 

	eventManager.AddListener(this, FusionEngine::EVENT_ON_CLICK);
}

void TextControl::Draw(glutil::MatrixStack &modelMatrix) const
{
	if (isVisible)
	{
		glutil::MatrixStack identityMatrix;
		identityMatrix.SetIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (hasBackground)
		{
			background.Draw(identityMatrix);
		}
		text.Draw();

		glDisable(GL_BLEND);
	}
}

bool TextControl::HandleEvent(const IEventData &eventData)
{
	Control::HandleEvent(eventData);

	EventType type = eventData.GetType();
	switch (type)
	{
	case EVENT_ON_RESHAPE:
		{
			textPosition = glm::vec2(position.x - margins.x,
									 position.y - margins.y);
			text.SetPosition(textPosition, windowWidth, windowHeight);
		}
		break;
	case EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);

			if (data.isLeftButtonDown && data.objectId == name)
			{
				if (handleOnClick != nullptr)
				{
					handleOnClick(this, eventData);
				}
				else
				{
					std::string errorMessage = "no OnClick handler for Control ==* ";
					errorMessage += name + " *==";
					HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
					return false;
				}
			}
		}
		break;
	}
	
	return false;
}


/////////////
//  Label  //
/////////////
bool Label::HandleEvent(const IEventData &eventData)
{
	Control::HandleEvent(eventData);

	EventType type = eventData.GetType();
	switch(type)
	{
		case EVENT_ON_RESHAPE:
			{
				textPosition = glm::vec2(position.x - margins.x,
										 position.y - margins.y);
				text.SetPosition(textPosition, windowWidth, windowHeight);
			}
			break;
	}

	return false;
}


//////////////
//  Button  //
//////////////
bool Button::HandleEvent(const IEventData &eventData)
{
	return TextControl::HandleEvent(eventData);
}


///////////////
//  TextBox  //
///////////////
const char ASCII_BACKSPACE = 8;

 void TextBox::Init(EventManager &eventManager)
{
	TextControl::Init(eventManager);

	visibleText = textString;

	eventManager.AddListener(this, FusionEngine::EVENT_ON_KEY_PRESSED);
}

bool TextBox::HandleEvent(const IEventData &eventData)
{
	TextControl::HandleEvent(eventData);

	EventType type = eventData.GetType();
	switch(type)
	{
		case EVENT_ON_KEY_PRESSED:
			{
				if (isActive)
				{
					char ch = static_cast<const OnKeyPressedEvent&>(eventData).key;

					if (ch == ASCII_BACKSPACE && textString.length() > 0)
					{
						textString.pop_back();
						int newTextLength = textString.length() - visibleText.length();
						if (newTextLength >= 0 && fabsf(text.GetWidth()) < maxWidth)
						{
							for (int i = visibleText.length() - 1; i >= 1; i--)
							{
								visibleText[i] = visibleText[i - 1];
							}
							visibleText[0] = textString[newTextLength];
						}
						else 
						{
							visibleText.pop_back();
						}
					}
					else
					{
						textString += ch;
						visibleText.push_back(ch);
					}

					while (fabsf(text.GetWidth()) > maxWidth && ch != ASCII_BACKSPACE)
					{
						visibleText.erase(visibleText.begin());
						text.CalculateTextWidth(visibleText);
					}

					text = Text(textFont, visibleText, textPosition, textColor, textSize);
					text.Init(windowWidth, windowHeight);
				}
			}
			break;
	}

	return false;
}


////////////////
//  ImageBox  //
////////////////
void ImageBox::Init(EventManager &eventManager)
{
	Control::Init(eventManager);

	eventManager.AddListener(this, FusionEngine::EVENT_ON_KEY_PRESSED);
}

bool ImageBox::HandleEvent(const IEventData &eventData)
{
	Control::HandleEvent(eventData);

	EventType type = eventData.GetType();
	switch(type)
	{
		case EVENT_ON_KEY_PRESSED:
			{
				if (handleOnKeyPress != nullptr)
				{
					handleOnKeyPress(this, eventData);
				}
				else 
				{
					std::string errorMessage = "no OnKeyPressed handler for Control ==* ";
					errorMessage += name + " *==";
					HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
					return false;
				}
			}

	}

	return false;
}

void ImageBox::ChangeImage(const std::string &newImageFile)
{
	background.Init(newImageFile);
}
#include "stdafx.h"
#include "Layout.h"

#include "../Fusion_EntitySystem/EntityEvents.h"


using namespace FusionEngine;


void Layout::Init(EventManager &eventManager)
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
	eventManager.AddListener(this, FusionEngine::EVENT_ON_KEY_PRESSED);
}

void Layout::AddControl(const std::shared_ptr<Control> newControl)
{
	controls.push_back(newControl);
}
std::vector<std::shared_ptr<Control>> Layout::GetControls() const
{
	return controls;
}

void Layout::RemoveListeners(EventManager &eventManager)
{
	eventManager.RemoveListener(this, FusionEngine::EVENT_ON_RESHAPE);
	eventManager.RemoveListener(this, FusionEngine::EVENT_ON_KEY_PRESSED);
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		eventManager.RemoveListener((*control).get(), FusionEngine::EVENT_ON_CLICK);
		eventManager.RemoveListener((*control).get(), FusionEngine::EVENT_ON_RESHAPE);
		eventManager.RemoveListener((*control).get(), FusionEngine::EVENT_ON_KEY_PRESSED);
	}
}

void Layout::Draw(glutil::MatrixStack &modelMatrix) const
{
	if (isBackgroundSet)
	{
		glutil::MatrixStack identityMatrix;
		identityMatrix.SetIdentity();

		background.Draw(identityMatrix);
	}
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		(*control)->Draw(modelMatrix);
	}
}

bool Layout::IsSet() const
{
	return isSet;
}
void Layout::Set(bool newIsSet)
{
	isSet = newIsSet;
}

bool Layout::HasActiveTextBox() const
{
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		if ((*control)->IsTextBox() && (*control)->IsActive())
		{
			return true;
		}
	}

	return false;
}

void Layout::SetBackgroundSprite(const std::string &spriteFile)
{
	if (background.GetWidth() > 0 || background.GetHeight() > 0)
	{
		background.Init(spriteFile);
		isBackgroundSet = true;
	}
	else
	{
		std::ostringstream errorMsg;
		errorMsg << "invalid background for layout " << (int)type;
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return;
	}
}

void Layout::SetOnKeyPressedHandler(EventHandlingFunc newOnKeyPressedHandler)
{
	onKeyPressedHandler = newOnKeyPressedHandler;
}

void Layout::DeactivateAllControls()
{
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		(*control)->SetActive(false);
	}
}

bool Layout::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case EVENT_ON_RESHAPE:
		{
			const OnReshapeEvent &data = static_cast<const OnReshapeEvent&>(eventData);
			
			background.SetPosition(glm::vec2(data.windowWidth, data.windowHeight));
			background.SetDimensions(data.windowWidth, data.windowHeight);
		}
		break;
	case EVENT_ON_KEY_PRESSED:
		{
			const OnKeyPressedEvent &data = static_cast<const OnKeyPressedEvent&>(eventData);
			
			if (onKeyPressedHandler)
			{
				onKeyPressedHandler(nullptr, data);
			}
		}
		break;
	}
	
	return false;
}
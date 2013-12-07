#include "stdafx.h"
#include "Layout.h"

#include "../Fusion_EntitySystem/EntityEvents.h"


using namespace FusionEngine;


void Layout::Init(EventManager &eventManager)
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

void Layout::AddControl(const std::shared_ptr<Control> newControl)
{
	controls.push_back(newControl);
}
std::vector<std::shared_ptr<Control>> Layout::GetControls()
{
	return controls;
}

void Layout::Draw(glutil::MatrixStack &modelMatrix)
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

bool Layout::IsSet()
{
	return isSet;
}
void Layout::Set(bool newIsSet)
{
	isSet = newIsSet;
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

/*
void Layout::SetControlOnClickHandler(const std::string &controlName,
									  OnClickHandler onClickHandler)
{
	for (auto control = controls.begin(); control != controls.end(); ++control)
	{
		if ((*control)->GetName() == controlName)
		{
			(*control)->SetOnClickHandler(onClickHandler);
		}
	}
}
*/

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
	}
	
	return false;
}
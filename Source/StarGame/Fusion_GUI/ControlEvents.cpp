#include "stdafx.h"
#include "ControlEvents.h"

#include "../Fusion_Scene/World.h"

using namespace FusionEngine;

const char ASCII_CHAR_ESCAPE = 27;


void Click_NewGame(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_GAME);
}
void Click_ResumeGame(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_GAME);
}
void Click_SaveGame(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_SAVE);
}
void Click_LoadGame(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_LOAD);
}
void Click_Options(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_OPTIONS);
}
void Click_Back(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	GetWorld().SetLayout(FE_LAYOUT_MENU);
}
void Click_Quit(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
	exit(0);
}
void Click_Apply(FusionEngine::Control *control, const FusionEngine::IEventData &eventData)
{
	const TextBox &cheatBox = static_cast<TextBox&>(*control);
	std::printf("Text box output: %s\n", cheatBox.GetText().c_str());
}
void Click_TextBox(FusionEngine::Control *control, const IEventData &eventData)
{
	GetWorld().GetCurrentLayout()->DeactivateAllControls();
	control->SetActive(true);
}

void KeyPressed_FusionImageBox(FusionEngine::Control *control,
							   const FusionEngine::IEventData &eventData)
{
	if (GetWorld().GetCurrentLayout()->HasActiveTextBox() || 
		GetWorld().GetCurrentLayout()->GetType() != FE_LAYOUT_GAME)
	{
		return;
	}

	char key = static_cast<const OnKeyPressedEvent&>(eventData).key;
	std::string objId = static_cast<const OnKeyPressedEvent&>(eventData).objectId;

	if (objId == control->GetName())
	{
		size_t imageIdx = 0;
		const std::string validInputButtons = GetWorld().GetFusionInputValidButtons();
		if (key == validInputButtons[0])
		{
			imageIdx = 1;
		}
		else if (key == validInputButtons[1])
		{
			imageIdx = 2;
		}
		else if (key == validInputButtons[2])
		{
			imageIdx = 3;
		}
		else if (key == GetWorld().GetFusionInputSequenceEndButton())
		{
			objId = "all";
		}

		control->ChangeBackgroundImage(imageIdx);
	}

	if (objId == "all")
	{
		Layout *currentLayout = GetWorld().GetCurrentLayout();
		currentLayout->GetControl("fusionOne")->ChangeBackgroundImage(0);
		currentLayout->GetControl("fusionTwo")->ChangeBackgroundImage(0);
		currentLayout->GetControl("fusionThree")->ChangeBackgroundImage(0);
	}
}

void KeyPressed_GameLayout(FusionEngine::Control *control, const FusionEngine::IEventData &eventData)
{
	char key = static_cast<const OnKeyPressedEvent&>(eventData).key;
	
	if (key == ASCII_CHAR_ESCAPE)
	{
		if (GetWorld().GetCurrentLayout()->GetType() != FE_LAYOUT_MENU)
		{
			if (GetWorld().GetCurrentLayout()->HasActiveTextBox())
			{
				GetWorld().GetCurrentLayout()->DeactivateAllControls();			
				return;
			}

			Control *resumeGame = GetWorld().GetLayout(FE_LAYOUT_MENU)->GetControl("resumeGame");
			Control *newGame = GetWorld().GetLayout(FE_LAYOUT_MENU)->GetControl("newGame");
			Control *loadGame = GetWorld().GetLayout(FE_LAYOUT_MENU)->GetControl("loadGame");
			Control *saveGame = GetWorld().GetLayout(FE_LAYOUT_MENU)->GetControl("saveGame");
			if (resumeGame && newGame && loadGame && saveGame)
			{
				resumeGame->SetVisibility(true);
				saveGame->SetVisibility(true);
				newGame->SetVisibility(false);
				loadGame->SetVisibility(false);
			}
			GetWorld().GetCurrentLayout()->DeactivateAllControls();
			GetWorld().SetLayout(FE_LAYOUT_MENU);
		}
		else
		{
			exit(0);
			return;
		}
	}
}

/*
const int ASCII_BACKSPACE = 8;

void KeyPressed_TextBox(FusionEngine::Control *control, const FusionEngine::IEventData &eventData)
{
	TextBox &tBox = static_cast<TextBox&>(*control);
	if (tBox.isActive)
	{
		char ch = static_cast<const OnKeyPressedEvent&>(eventData).key;

		if ((int)ch == ASCII_BACKSPACE && tBox.textString.length() > 0)
		{
			tBox.textString.pop_back();
			int newTextLength = tBox.textString.length() - tBox.visibleText.length();
			if (newTextLength >= 0 && fabsf(tBox.text.GetWidth()) < tBox.maxWidth)
			{
				for (int i = tBox.visibleText.length() - 1; i >= 1; i--)
				{
					tBox.visibleText[i] = tBox.visibleText[i - 1];
				}
				tBox.visibleText[0] = tBox.textString[newTextLength];
			}
			else 
			{
				tBox.visibleText.pop_back();
			}
		}
		else
		{
			tBox.textString += ch;
			tBox.visibleText.push_back(ch);
		}

		while (fabsf(tBox.text.GetWidth()) > tBox.maxWidth && (int)ch != ASCII_BACKSPACE)
		{
			tBox.visibleText.erase(tBox.visibleText.begin());
			tBox.text.CalculateTextWidth(tBox.visibleText);
		}

		tBox.text = 
			Text(tBox.textFont, tBox.visibleText, tBox.textPosition, 
				 tBox.textColor, tBox.textSize);
		tBox.text.Init(tBox.windowWidth, tBox.windowHeight);
	}
}
*/
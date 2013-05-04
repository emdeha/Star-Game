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


// TODO: Use smart pointers where needed.
// TODO: Find a better way for the 'gamma' value to be distributed.
// TODO: Place 'CorrectGamma' in 'Utility'
// TODO: Make sure everything compiles with the include files removed from 'Microsoft SDKs'
// TODO: DATA DRIVEN DESIGN!!!
// TODO: Better error handling.
// TODO: Fix a bug with satellite selection. 
//		 When the sun is moved the selection doesn't work properly. Move the sun for an example.

#include <ctime>


#include "Main.h"
#include "ShaderManager.h"
#include "DisplayData.h"
#include "../framework/EventSystem.h"
#include "../Scene/Scene.h"
#include "../AssetLoader/GUILoader.h"
#include "../AssetLoader/MeshLoader.h"
#include "../ParticleEngine/Engine.h"
#include "../Entities/Skills.h"
#include "../GUISystem/GameSpecificGUI.h"
#include "../dirent/dirent.h"
#include "../framework/ErrorAPI.h"


DisplayData displayData;

Scene scene = Scene(2.2f, 8.0f, 3.0f, 0.5f, 0, 4, 20.0f);


SpriteParticleEmitter testSpriteEmitter;

//Control testControl;


long long GetCurrentTimeMillis()
{
	return time(0) * 1000;
}


void HandleMouse()
{
	glm::vec4 cameraPosition = glm::vec4(scene.GetTopDownCamera().ResolveCamPosition(), 1.0f);
	
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	Utility::Ray mouseRay = 
		scene.GetMouse().GetPickRay(displayData.projectionMatrix, displayData.modelMatrix,
									cameraPosition, 
									windowWidth, windowHeight);

	if(scene.GetMouse().IsRightButtonDown())
	{
		Event rightClickEvent = StockEvents::EventOnRightClick("all", 
															   scene.GetMouse().GetCurrentPosition());

		scene.OnEvent(rightClickEvent);
	}
	if(scene.GetMouse().IsLeftButtonDown())
	{
		Event leftClickEvent = StockEvents::EventOnLeftClick("all",
															 scene.GetMouse().GetCurrentPosition());

		scene.OnEvent(leftClickEvent);
	}

	if(scene.GetMouse().IsRightButtonDown())
	{		
		if(scene.HasSuns())
		{
			if(scene.GetSun()->IsClicked(mouseRay))
			{
				Event rightClickSunEvent = StockEvents::EventOnRightClick("sun");

				scene.GetSun()->OnEvent(rightClickSunEvent);
				scene.OnEvent(rightClickSunEvent);
				scene.OnEvent(scene.GetSun()->GetGeneratedEvent("satelliteRemoved"));
				scene.GetSun()->RemoveGeneratedEvent("satelliteRemoved");
			}
		}
	}

	if(scene.GetMouse().IsLeftButtonDown())
	{
		if(scene.IsLayoutOn(LAYOUT_IN_GAME))
		{
			scene.OnEvent(StockEvents::EventOnLeftClick("deploySkill"));
		}
		if(scene.HasSuns())
		{
			std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
			for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin(); 
				iter != sunSatellites.end(); ++iter)
			{
				if((*iter)->IsClicked(mouseRay) && (*iter)->GetIsDrawingUpgradeButtons())
				{
					int buttonIndex = 0;
					CelestialBody::SkillButtonClickedData buttonClickedData = 
						(*iter)->IsSkillUpgradeButtonClicked(mouseRay, buttonIndex);
					bool isUpgrBoxClicked = buttonClickedData.isClicked;
					if(isUpgrBoxClicked && scene.GetMouse().IsLeftButtonDown())
					{
						EventArg upgradeSkillClickedEventArgs[3];
						upgradeSkillClickedEventArgs[0].argType = "what_event";
						upgradeSkillClickedEventArgs[0].argument.varType = TYPE_STRING;
						strcpy(upgradeSkillClickedEventArgs[0].argument.varString, "skillUpgr");
						upgradeSkillClickedEventArgs[1].argType = "index";
						upgradeSkillClickedEventArgs[1].argument.varType = TYPE_INTEGER;
						upgradeSkillClickedEventArgs[1].argument.varInteger = buttonIndex;
						upgradeSkillClickedEventArgs[2].argType = "satType";
						upgradeSkillClickedEventArgs[2].argument.varType = TYPE_INTEGER;
						upgradeSkillClickedEventArgs[2].argument.varInteger = (*iter)->GetSatelliteType();
						Event upgradeSkillClickedEvent(3, EVENT_TYPE_OTHER, upgradeSkillClickedEventArgs);

						//(*iter)->OnEvent(upgradeSkillClickedEvent);
						scene.OnEvent(upgradeSkillClickedEvent);
					}
					Event satelliteHoveredEvent = StockEvents::EventOnHover();

					(*iter)->OnEvent(satelliteHoveredEvent);
					// WARN: This would emit an unexpected error because the scene doesn't handle on hover events
					//scene.OnEvent(satelliteHoveredEvent);
				}
			}

			if(scene.GetSun()->IsClicked(mouseRay) && scene.GetSun()->GetIsDrawingUpgradeButtons())
			{
				int buttonIndex = 0;
				CelestialBody::SkillButtonClickedData buttonClickedData = 
					scene.GetSun()->IsSkillUpgradeButtonClicked(mouseRay, buttonIndex);
				bool isUpgrBoxClicked = buttonClickedData.isClicked;
				if(isUpgrBoxClicked && scene.GetMouse().IsLeftButtonDown())
				{
					EventArg upgradeSkillClickedEventArgs[4];
					upgradeSkillClickedEventArgs[0].argType = "what_event";
					upgradeSkillClickedEventArgs[0].argument.varType = TYPE_STRING;
					strcpy(upgradeSkillClickedEventArgs[0].argument.varString, "skillUpgr");
					upgradeSkillClickedEventArgs[1].argType = "index";
					upgradeSkillClickedEventArgs[1].argument.varType = TYPE_INTEGER;
					upgradeSkillClickedEventArgs[1].argument.varInteger = buttonIndex;
					upgradeSkillClickedEventArgs[2].argType = "satType";
					upgradeSkillClickedEventArgs[2].argument.varType = TYPE_INTEGER;
					upgradeSkillClickedEventArgs[2].argument.varInteger = -1;
					upgradeSkillClickedEventArgs[3].argType = "skillIndex";
					upgradeSkillClickedEventArgs[3].argument.varType = TYPE_INTEGER;
					upgradeSkillClickedEventArgs[3].argument.varInteger = buttonClickedData.skillId;
					Event upgradeSkillClickedEvent(4, EVENT_TYPE_OTHER, upgradeSkillClickedEventArgs);

					//(*iter)->OnEvent(upgradeSkillClickedEvent);
					scene.OnEvent(upgradeSkillClickedEvent);
				}
				Event sunHoveredEvent = StockEvents::EventOnHover();

				scene.GetSun()->OnEvent(sunHoveredEvent);
				// WARN: This would emit an unexpected error because the scene doesn't handle on hover events
				//scene.OnEvent(sunHoveredEvent);
			}
		}
		/*
		if(scene.IsLayoutOn(LAYOUT_LOAD_GAME))
		{
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->
				GetControl("backBtn")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("backButton");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("backBtn")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->IsControl("saveSlot1") &&
				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot1")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadSlot1");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot1")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->IsControl("saveSlot2") &&
				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot2")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadSlot2");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot2")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->IsControl("saveSlot3") &&
				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot3")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadSlot3");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot3")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->IsControl("saveSlot4") &&
				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot4")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadSlot4");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot4")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_LOAD_GAME)->IsControl("saveSlot5") &&
				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot5")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadSlot5");

				scene.GetLayout(LAYOUT_LOAD_GAME)->GetControl("saveSlot5")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
		}
		if(scene.IsLayoutOn(LAYOUT_SAVE_GAME))
		{
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("backBtn")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("backButton");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("backBtn")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("saveSlot1")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveSlot1");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("saveSlot1")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("saveSlot2")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveSlot2");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("saveSlot2")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("saveSlot3")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveSlot3");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("saveSlot3")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("saveSlot4")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveSlot4");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("saveSlot4")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			if(
				scene.GetLayout(LAYOUT_SAVE_GAME)->
				GetControl("saveSlot5")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveSlot5");

				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl("saveSlot5")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
		}
		if(scene.IsLayoutOn(LAYOUT_OPTIONS))
		{
			if(
				scene.GetLayout(LAYOUT_OPTIONS)->
				GetControl("backBtn")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("backButton");

				scene.GetLayout(LAYOUT_OPTIONS)->GetControl("backBtn")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
		}

		if(scene.IsLayoutOn(LAYOUT_MENU))
		{
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("newGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("newGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("newGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}
			else
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("resumeGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				std::printf("resume");

				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("resumeGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("resumeGame")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			else
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("saveGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("saveGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}
			else
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("loadGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				std::printf("load");

				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("loadGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("loadGame")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			else
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("options")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("options");

				scene.GetLayout(LAYOUT_MENU)->GetControl("options")->OnEvent(leftClickButtonEvent);
				scene.OnEvent(leftClickButtonEvent);
			}
			else
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("quitGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("quitGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("quitGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}
		}
		*/
		if(scene.IsLayoutOn(LAYOUT_IN_GAME))
		{
			if(scene.HasSuns())
			{
				if(scene.GetSun()->IsClicked(mouseRay))
				{
					Event leftClickSunEvent = StockEvents::EventOnLeftClick("sun");

					scene.GetSun()->OnEvent(leftClickSunEvent);
					scene.OnEvent(leftClickSunEvent);
				}

				std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
				for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin(); 
					iter != sunSatellites.end(); ++iter)
				{
					if((*iter)->IsClicked(mouseRay))
					{
						Event leftClickSatelliteEvent = StockEvents::EventOnLeftClick("satellite");

						(*iter)->OnEvent(leftClickSatelliteEvent);
						scene.OnEvent(leftClickSatelliteEvent);
					}
				}
			}
			/*
			if(scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->
			   IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickTextBoxEvent = StockEvents::EventOnLeftClick("varInput");

				scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->OnEvent(leftClickTextBoxEvent);

				if(scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput") !=
				   scene.GetLayout(LAYOUT_IN_GAME)->GetActiveControl())
				{
					Event unclickEvent = Event(EVENT_TYPE_UNCLICK);
					scene.GetLayout(LAYOUT_MENU)->GetActiveControl()->OnEvent(unclickEvent);
				}
			}

			if(scene.GetLayout(LAYOUT_IN_GAME)->
			   GetControl("applyInput")->
			   IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("applyInput");

				scene.GetLayout(LAYOUT_IN_GAME)->GetControl("applyInput")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}*/
		}
	}

	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{
		std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			if((*iter)->IsClicked(mouseRay))
			{
				Event satelliteHoveredEvent = StockEvents::EventOnHover();

				(*iter)->OnEvent(satelliteHoveredEvent);
			}
			else 
			{
				EventArg satelliteUnhoveredEventArg[1];
				satelliteUnhoveredEventArg[0].argType = "what_event";
				satelliteUnhoveredEventArg[0].argument.varType = TYPE_STRING;
				strcpy(satelliteUnhoveredEventArg[0].argument.varString, "satUnhov");
				Event satelliteUnhoveredEvent(1, EVENT_TYPE_OTHER, satelliteUnhoveredEventArg);

				(*iter)->OnEvent(satelliteUnhoveredEvent);
			}
		}

		if(scene.GetSun()->IsClicked(mouseRay))
		{
			Event sunHoveredEvent = StockEvents::EventOnHover();

			scene.GetSun()->OnEvent(sunHoveredEvent);
		}
		else 
		{
			EventArg sunUnhoveredEventArg[1];
			sunUnhoveredEventArg[0].argType = "what_event";
			sunUnhoveredEventArg[0].argument.varType = TYPE_STRING;
			strcpy(sunUnhoveredEventArg[0].argument.varString, "sunUnhov");
			Event sunUnhoveredEvent(1, EVENT_TYPE_OTHER, sunUnhoveredEventArg);

			scene.GetSun()->OnEvent(sunUnhoveredEvent);
		}

		bool isBodySkillHovered = false;

		//std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin(); 
			iter != sunSatellites.end(); ++iter)
		{
			if((*iter)->IsClicked(mouseRay) && (*iter)->GetIsDrawingUpgradeButtons())
			{
				int buttonIndex = 0;
				bool isUpgrBoxClicked = (*iter)->IsSkillUpgradeButtonClicked(mouseRay, buttonIndex).isClicked;
				if(isUpgrBoxClicked)
				{
					EventArg upgradeSkillHoveredEventArgs[3];
					upgradeSkillHoveredEventArgs[0].argType = "what_event";
					upgradeSkillHoveredEventArgs[0].argument.varType = TYPE_STRING;
					strcpy(upgradeSkillHoveredEventArgs[0].argument.varString, "skillHov");
					upgradeSkillHoveredEventArgs[1].argType = "index";
					upgradeSkillHoveredEventArgs[1].argument.varType = TYPE_INTEGER;
					upgradeSkillHoveredEventArgs[1].argument.varInteger = buttonIndex;
					upgradeSkillHoveredEventArgs[2].argType = "satType";
					upgradeSkillHoveredEventArgs[2].argument.varType = TYPE_INTEGER;
					upgradeSkillHoveredEventArgs[2].argument.varInteger = (*iter)->GetSatelliteType();
					Event upgradeSkillHoveredEvent(3, EVENT_TYPE_OTHER, upgradeSkillHoveredEventArgs);

					//(*iter)->OnEvent(upgradeSkillClickedEvent);
					scene.OnEvent(upgradeSkillHoveredEvent);
					isBodySkillHovered = true;
				}
				Event satelliteHoveredEvent = StockEvents::EventOnHover();

				(*iter)->OnEvent(satelliteHoveredEvent);
				// WARN: This would emit an unexpected error because the scene doesn't handle on hover events
				//scene.OnEvent(satelliteHoveredEvent);
			}
		}

		if(scene.GetSun()->IsClicked(mouseRay) && scene.GetSun()->GetIsDrawingUpgradeButtons())
		{
			int buttonIndex = 0;
			bool isUpgrBoxClicked = scene.GetSun()->IsSkillUpgradeButtonClicked(mouseRay, buttonIndex).isClicked;
			if(isUpgrBoxClicked)
			{
				EventArg upgradeSkillHoveredEventArgs[3];
				upgradeSkillHoveredEventArgs[0].argType = "what_event";
				upgradeSkillHoveredEventArgs[0].argument.varType = TYPE_STRING;
				strcpy(upgradeSkillHoveredEventArgs[0].argument.varString, "skillHov");
				upgradeSkillHoveredEventArgs[1].argType = "index";
				upgradeSkillHoveredEventArgs[1].argument.varType = TYPE_INTEGER;
				upgradeSkillHoveredEventArgs[1].argument.varInteger = buttonIndex;
				upgradeSkillHoveredEventArgs[2].argType = "satType";
				upgradeSkillHoveredEventArgs[2].argument.varType = TYPE_INTEGER;
				upgradeSkillHoveredEventArgs[2].argument.varInteger = -1;
				Event upgradeSkillHoveredEvent(3, EVENT_TYPE_OTHER, upgradeSkillHoveredEventArgs);

				//(*iter)->OnEvent(upgradeSkillClickedEvent);
				scene.OnEvent(upgradeSkillHoveredEvent);
				isBodySkillHovered = true;
			}
			Event sunHoveredEvent = StockEvents::EventOnHover();

			scene.GetSun()->OnEvent(sunHoveredEvent);
			// WARN: This would emit an unexpected error because the scene doesn't handle on hover events
			//scene.OnEvent(sunHoveredEvent);
		}

		if(!isBodySkillHovered)
		{
			scene.GetLayout(LAYOUT_IN_GAME)->GetControl("labelToolTip")->SetIsVisible(false);
		}
	}

	if(scene.IsLayoutOn(LAYOUT_MENU))
	{
		/*if(
			universe->GetLayout(LAYOUT_MENU).
			GetButtonControl("newGame")->
			IsMouseOn(scene->GetMouse().GetClipSpacePosition(windowWidth, windowHeight))
			)
		{
			//Event hoverButtonEvent = StockEvents::EventOnHover();
			EventArg hoverButtonEventArg[1];
			hoverButtonEventArg[0].argType = "isBold";
			hoverButtonEventArg[0].argument.varType = TYPE_BOOL;
			hoverButtonEventArg[0].argument.varBool = true;

			Event hoverButtonEvent = Event(1, EVENT_TYPE_ON_HOVER, hoverButtonEventArg);

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->OnEvent(hoverButtonEvent);

			//universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->SetFont(true);
		}
		else
		{
			EventArg hoverButtonEventArg[1];
			hoverButtonEventArg[0].argType = "isBold";
			hoverButtonEventArg[0].argument.varType = TYPE_BOOL;
			hoverButtonEventArg[0].argument.varBool = false;

			Event hoverButtonEvent = Event(1, EVENT_TYPE_ON_HOVER, hoverButtonEventArg);

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->OnEvent(hoverButtonEvent);
		}

		/*if(
			universe->GetLayout(LAYOUT_MENU).
			GetButtonControl("saveGame").
			IsMouseOn(scene->GetMouse().GetClipSpacePosition(windowWidth, windowHeight))
			)
		{
			Event hoverButtonEvent = StockEvents::EventOnHover();

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("saveGame").OnEvent(hoverButtonEvent);
		}*/
	}


	displayData.mousePosition = scene.GetMouse().GetCurrentPosition();


	scene.GetMouse().ReleaseRightButton();
	scene.GetMouse().ReleaseLeftButton();
}
void HandleMouseButtons(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		scene.GetMouse().PressLeftButton();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		scene.GetMouse().ReleaseLeftButton();
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		scene.GetMouse().PressRightButton();
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		scene.GetMouse().ReleaseRightButton();
	}
}
void HandleActiveMovement(int x, int y)
{
	scene.GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}
void HandlePassiveMovement(int x, int y)
{
	scene.GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}

void InitializePrograms()
{
	scene.GetShaderManager().LoadLitProgram("shaders/PN.vert", "shaders/GaussianLighting.frag");
	scene.GetShaderManager().LoadUnlitProgram("shaders/PosTransform.vert", "shaders/UniformColor.frag");
	scene.GetShaderManager().LoadSimpleProgram("shaders/PosColorLocalTransform.vert", "shaders/ColorPassThrough.frag");
	scene.GetShaderManager().LoadSimpleNoUBProgram("shaders/PosTransformNoUB.vert", "shaders/ColorPassThrough.frag");
	scene.GetShaderManager().LoadFontProgram("shaders/Font.vert", "shaders/Font.frag");
	scene.GetShaderManager().LoadSimpleTextureProgData("shaders/SimpleTexture.vert", "shaders/SimpleTexture.frag");
	scene.GetShaderManager().LoadTextureProgData("shaders/Texture.vert", "shaders/Texture.frag");
	scene.GetShaderManager().LoadPerspectiveTextureProgData("shaders/TexturePerspective.vert", "shaders/Texture.frag");
	scene.GetShaderManager().LoadBillboardProgData("shaders/BillboardShader.vert", "shaders/BillboardShader.geom", "shaders/BillboardShader.frag");
	scene.GetShaderManager().LoadParticleProgData("shaders/ParticleShaderBillboarded.vert", "shaders/ParticleShaderBillboarded.geom");
	scene.GetShaderManager().LoadBillboardProgDataNoTexture("shaders/BillboardShader.vert", "shaders/BillboardShaderNoTexture.geom", "shaders/BillboardShaderNoTexture.frag");
	scene.GetShaderManager().LoadSpriteParticleProgData("shaders/ParticleShader.vert", "shaders/ParticleShader.frag");
	scene.GetShaderManager().LoadLitTextureProgram("shaders/LitTexture.vert", "shaders/LitTexture.frag");
	scene.GetShaderManager().LoadSpriteParticleProgData("shaders/SpriteParticleShader.vert", "shaders/SpriteParticleShader.frag");
}

void InitializeGUI()
{
	GUILoader guiLoader("../data/loader-files/test-gui.yaml", 
						glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	scene.AddLayouts(guiLoader.GetAllLoadedLayouts());
	
	scene.GetLayout(LAYOUT_IN_GAME)->GetControl("labelToolTip")->SetIsVisible(false);
	
	// Set buttons text
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	if ((dir = opendir ("../data/saved-games/")) != NULL) 
	{
		glm::vec2 controlPosition = glm::vec2(10, 600);
		// print all the files and directories within directory 
		while ((ent = readdir (dir)) != NULL) 
		{
			//printf ("%s\n", ent->d_name);
			if(ent->d_namlen > 5)
			{
				i++;
				std::string controlName = "saveSlot";
				controlName += (char)(i + 48); // converting number to its char
				controlPosition.y -= 30.0f;
				std::vector<std::string> splittedFileName = Utility::SplitString(ent->d_name, '.');
				scene.GetLayout(LAYOUT_SAVE_GAME)->GetControl(controlName)->SetText(splittedFileName[0]);

				std::shared_ptr<Button> loadEntryButton = 
					std::shared_ptr<Button>(new Button(splittedFileName[0], splittedFileName[0],
													   glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
													   controlPosition, glm::vec4(),
													   28,
													   false, true, false,
													   glm::vec2()));
				loadEntryButton->Init("../data/fonts/AGENCYR.TTF", "",
									  glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
				scene.GetLayout(LAYOUT_LOAD_GAME)->AddControl(loadEntryButton);
			}
		}
		closedir (dir);
	} 
	else 
	{
		HandleUnexpectedError("invalid directory", __LINE__, __FILE__);
	}
}
/*
void ControlActivationEventHandler(Scene &scene, Control *control)
{
	scene.GetLayout(LAYOUT_MENU)->DeactivateAllControls();
	scene.GetLayout(LAYOUT_IN_GAME)->DeactivateAllControls();
	control->SetIsActive(true);
}

void OnTextBoxEnterPressEventHandler(Scene &scene, Control *control)
{
	std::printf("Text: %s\n", control->GetContent().c_str());
}*/
void OnTextBoxClickedEventHandler(Scene &scene, Control *control)
{
	scene.GetLayout(LAYOUT_MENU)->DeactivateAllControls();
	scene.GetLayout(LAYOUT_IN_GAME)->DeactivateAllControls();
	control->SetIsActive(true);
}
void OnNewGameClickEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_MENU, false);
	scene.SetLayout(LAYOUT_IN_GAME, true);

	scene.ResetScene();
	scene.SetPause(false);

	EventArg inGameEventArg[1];
	inGameEventArg[0].argType = "command";
	inGameEventArg[0].argument.varType = TYPE_STRING;
	strcpy(inGameEventArg[0].argument.varString, "playBackgroundMusic");
	Event inGameEvent = Event(1, EVENT_TYPE_OTHER, inGameEventArg);

	scene.OnEvent(inGameEvent);
}
void OnResumeGameClickEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_MENU, false);
	scene.SetLayout(LAYOUT_IN_GAME, true);

	//scene.sceneMusic.Stop(CHANNEL_MASTER);
	//scene.sceneMusic.Play(MUSIC_BACKGROUND, CHANNEL_MASTER);
	scene.StopMusic(CHANNEL_MASTER);
	scene.PlayMusic(MUSIC_BACKGROUND, CHANNEL_MASTER);

	scene.StartScene();
}
void OnSaveGameClickedEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_SAVE_GAME, true);
	scene.SetLayout(LAYOUT_MENU, false);
}
void OnLoadGameClickedEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_LOAD_GAME, true);
	scene.SetLayout(LAYOUT_MENU, false);
}
void OnOptionsClickedEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_OPTIONS, true);
	scene.SetLayout(LAYOUT_MENU, false);
}
void OnBackButtonClickedEventHandler(Scene &scene, Control *control)
{
	scene.SetLayout(LAYOUT_ALL, false);
	scene.SetLayout(LAYOUT_MENU, true);
}
void OnApplyInputClickedEventHandler(Scene &scene, Control *control)
{
	if(scene.HasSuns())
	{
		scene.ProcessVariablesTweak(scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->GetContent());
	}

	scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->ClearContent();
	scene.GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->SetIsActive(false);
}
void OnQuitClickedEventHandler(Scene &scene, Control *control)
{
	// Add clean-up if necessary.
	exit(EXIT_SUCCESS);
}
void OnSaveSlotClickedEventHandler(Scene &scene, Control *control)
{
	if(control->GetContent() == "Empty")
	{
		std::string currentDateTime = Utility::GetCurrentDateTimeAsString("%Y-%m-%d-%H-%M-%S");
		control->SetText(currentDateTime);
		scene.SaveGame("../data/saved-games/" + currentDateTime + ".yaml");
	}
}

void InitializeScene()
{
	Mouse userMouse;
	TopDownCamera userCamera = TopDownCamera(glm::vec3(), 13.5f, 0.0f, 45.0f);


	std::shared_ptr<CelestialBody> 
		mainSun(new CelestialBody(glm::vec3(0.0f), glm::vec4(0.738f, 0.738f, 0.423f, 1.0f), 1.25f, 4, 50, 
								  200, 100));

	SunLight 
		mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 1.2f, 5.0f, displayData.gamma));

	mainSun->LoadMesh("../data/mesh-files/sun.obj");

	scene.SetMouse(userMouse);
	scene.SetTopDownCamera(userCamera);

	scene.AddSun(mainSun);
	scene.AddSunLight(mainSunLight);

	ExplosionEmitter sceneExplosion = ExplosionEmitter(glm::vec3(), 300, 30, 0.1f);
	sceneExplosion.Init();
	scene.SetExplosion(sceneExplosion);

	scene.SetFusion(FusionInput('f'));
	scene.AddFusionSequence("fireSatellite", 'q', 'q', 'q');
	scene.AddFusionSequence("earthSatellite", 'e', 'e', 'e');
	scene.AddFusionSequence("waterSatellite", 'w', 'w', 'w');
	scene.AddFusionSequence("airSatellite", 'q', 'w', 'e');
	scene.AddFusionSequence("aoeSkill", 'q', 'q', 'w');
	scene.AddFusionSequence("passiveAoeSkill", 'q', 'q', 'e');
	scene.AddFusionSequence("sunNovaSkill", 'w', 'w', 'e');
	scene.AddFusionSequence("satFrostNova", 'q', 'w', 'q');
	scene.AddFusionSequence("satShieldSkill", 'w', 'e', 'w');
	scene.AddFusionSequence("burnSkill", 'w', 'e', 'q');
	

	glUseProgram(scene.GetShaderManager().GetTextureProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetTextureProgData().colorTextureUnif, 0);
	glUseProgram(0);
	glUseProgram(scene.GetShaderManager().GetPerspectiveTextureProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetPerspectiveTextureProgData().colorTextureUnif, 0);
	glUseProgram(0);
	glUseProgram(scene.GetShaderManager().GetSimpleTextureProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetSimpleTextureProgData().textureUnif, 0);
	glUseProgram(0);
	glUseProgram(scene.GetShaderManager().GetLitTextureProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetLitTextureProgData().textureUnif, 0);
	glUseProgram(0);
	glUseProgram(scene.GetShaderManager().GetSpriteParticleProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetSpriteParticleProgData().samplerUnif, 0);
	glUseProgram(0);
	

	// WARN: When audio is not loaded first, a break is thrown
	scene.LoadAudio("../data/loader-files/audio-config.yaml");
	scene.InitTweakableVariables(true, "../data/loader-files/tweak-config.yaml");
	InitializeGUI();

	//scene.GetLayout(LAYOUT_IN_GAME)->SetBackgroundImage(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
	//													"../data/images/background.png");

	EventArg inMenuEventArg[1];
	inMenuEventArg[0].argType = "command";
	inMenuEventArg[0].argument.varType = TYPE_STRING;
	strcpy(inMenuEventArg[0].argument.varString, "playMenuMusic");
	Event inMenuEvent = Event(1, EVENT_TYPE_OTHER, inMenuEventArg);

	scene.OnEvent(inMenuEvent);
	
	scene.AddEventHandler("onNewGameClickedEventHandler", "newGame", "",
						  LAYOUT_MENU, OnNewGameClickEventHandler);
	scene.AddEventHandler("onResumeGameClickedEventHandler", "resumeGame", "", 
						  LAYOUT_MENU, OnResumeGameClickEventHandler);
	scene.AddEventHandler("onSaveGameClickedEventHandler", "saveGame", "", 
						  LAYOUT_MENU, OnSaveGameClickedEventHandler);
	scene.AddEventHandler("onLoadGameClickedEventHandler", "loadGame", "", 
						  LAYOUT_MENU, OnLoadGameClickedEventHandler);
	scene.AddEventHandler("onOptionsClickedEventHandler", "options", "", 
						  LAYOUT_MENU, OnOptionsClickedEventHandler);
	scene.AddEventHandler("onBackButtonClickedEventHandler", "backBtn", "", 
						  LAYOUT_ALL, OnBackButtonClickedEventHandler);
	scene.AddEventHandler("onApplyInputClickedEventHandler", "applyInput", "", 
						  LAYOUT_IN_GAME, OnApplyInputClickedEventHandler);
	scene.AddEventHandler("onQuitClickedEventHandler", "quitGame", "", 
						  LAYOUT_MENU, OnQuitClickedEventHandler);
	scene.AddEventHandler("onTextBoxClickedEventHandler", "", "TextBox", 
						  LAYOUT_ALL, OnTextBoxClickedEventHandler);

	scene.AddEventHandler("onSaveSlotOneClickedEventHandler", "saveSlot1", "", 
						  LAYOUT_SAVE_GAME, OnSaveSlotClickedEventHandler);
	scene.AddEventHandler("onSaveSlotOneClickedEventHandler", "saveSlot2", "", 
						  LAYOUT_SAVE_GAME, OnSaveSlotClickedEventHandler);
	scene.AddEventHandler("onSaveSlotOneClickedEventHandler", "saveSlot3", "", 
						  LAYOUT_SAVE_GAME, OnSaveSlotClickedEventHandler);
	scene.AddEventHandler("onSaveSlotOneClickedEventHandler", "saveSlot4", "", 
						  LAYOUT_SAVE_GAME, OnSaveSlotClickedEventHandler);
	scene.AddEventHandler("onSaveSlotOneClickedEventHandler", "saveSlot5", "", 
						  LAYOUT_SAVE_GAME, OnSaveSlotClickedEventHandler);
}


const int TICKS_PER_SECOND = 25;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

unsigned long nextGameTick = 0;




unsigned int frameCount = 0;
const char WINDOW_TITLE[] = "Test Playground: ";

void TimerFunction(int value)
{
	if(value != 0)
	{
		char *tempString = (char *)malloc(512 + strlen(WINDOW_TITLE));

		sprintf(tempString, "%s %i frames per second", WINDOW_TITLE, frameCount * 4);

		glutSetWindowTitle(tempString);
		free(tempString);
	}

	frameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}

long long currentTime_milliseconds;

PassiveAoeAnimation testPassiveAoeAnim;

void Init()
{
	currentTime_milliseconds = GetCurrentTimeMillis();

	glutTimerFunc(0, TimerFunction, 0);


	InitializePrograms();
	InitializeScene();

	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	const float depthZNear = 0.0f;
	const float depthZFar = 1.0f;

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(depthZNear, depthZFar);
	glEnable(GL_DEPTH_CLAMP);

	GLuint lightUniformBuffer = 0;
	glGenBuffers(1, &lightUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_LIGHT, lightUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

	GLuint projectionUniformBuffer = 0;
	glGenBuffers(1, &projectionUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_PROJECTION, projectionUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	GLuint orthographicUniformBuffer = 0;
	glGenBuffers(1, &orthographicUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_ORTHOGRAPHIC, orthographicUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, orthographicUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	// Bind the static buffers.
	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_LIGHT), 
		lightUniformBuffer, 
		0, sizeof(LightBlockGamma));

	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_PROJECTION), 
		projectionUniformBuffer,
		0, sizeof(glm::mat4));

	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_ORTHOGRAPHIC),
		orthographicUniformBuffer,
		0, sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	nextGameTick = GetTickCount();
}

bool isEmitterStarted = false;

void Display()
{
	frameCount++;

	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{		
		scene.SetDisplayData(displayData);
		
		glutil::MatrixStack modelMatrix;

		modelMatrix.SetMatrix(scene.GetTopDownCamera().CalcMatrix());
		displayData.modelMatrix = modelMatrix.Top();

		int loops = 0;
		while(GetTickCount() > nextGameTick && loops < MAX_FRAMESKIP)
		{
			scene.UpdateScene();

			nextGameTick += SKIP_TICKS;
			loops++;
		}

		float interpolation = float(GetTickCount() + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);
		scene.RenderScene(modelMatrix, interpolation);	
		
		scene.RenderCurrentLayout();	

		if(isEmitterStarted)
		{
			
		}
	}
	else //if(scene->IsLayoutOn(LAYOUT_MENU))
	{
		if(scene.IsPaused())
		{
			int loops = 0;
			while(GetTickCount() > nextGameTick && loops < MAX_FRAMESKIP)
			{
				scene.UpdateScene();

				nextGameTick += SKIP_TICKS;
				loops++;
			}
		}
		scene.RenderCurrentLayout();
	}

	HandleMouse();
	scene.GetMouse().OverrideLastPosition(scene.GetMouse().GetCurrentPosition());
	
	glutSwapBuffers();
	glutPostRedisplay();
}


void Reshape(int width, int height)
{
	glutil::MatrixStack projMatrix;
	float aspectRatio = width / (float)height;
	projMatrix.Perspective(45.0f, aspectRatio, displayData.zNear, displayData.zFar);

	displayData.projectionMatrix = projMatrix.Top();
	
	glUseProgram(scene.GetShaderManager().GetBillboardProgData().theProgram);
	glUniformMatrix4fv(scene.GetShaderManager().GetBillboardProgData().cameraToClipMatrixUnif, 
					   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);

	glUseProgram(scene.GetShaderManager().GetBillboardProgDataNoTexture().theProgram);
	glUniformMatrix4fv(scene.GetShaderManager().GetBillboardProgDataNoTexture().cameraToClipMatrixUnif,		
					   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);
	
	glBindBuffer(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetUniformBuffer(UBT_PROJECTION));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &displayData.projectionMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	projMatrix.SetIdentity();
	projMatrix.Orthographic((float)width, 0.0f, (float)height, 0.0f, 1.0f, 1000.0f);
	
	glBindBuffer(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetUniformBuffer(UBT_ORTHOGRAPHIC));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &projMatrix.Top());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	scene.UpdateCurrentLayout(width, height);

	displayData.windowHeight = height;
	displayData.windowWidth = width;

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	if(scene.IsLayoutOn(LAYOUT_IN_GAME) &&
	   scene.GetLayout(LAYOUT_IN_GAME)->HasActiveControl())
	{
		if((int)key == 13)
		{
			Event buttonPressedEvent = 
				StockEvents::EventOnButtonPressed(
					key, scene.GetLayout(LAYOUT_IN_GAME)->GetActiveControl()->GetName().c_str());
			scene.OnEvent(buttonPressedEvent);
			scene.GetLayout(LAYOUT_IN_GAME)->GetActiveControl()->ClearContent();
			return;
		}
		scene.GetLayout(LAYOUT_IN_GAME)->GetActiveControl()->InputChar(key);
		return;
	}

	switch(key)
	{
	/*
	// exit the game
	case 27:
		// TODO: When exiting that way, if the scene is not a pointer, 
		//	     the application crashes.
		glutLeaveMainLoop();
		return;
	*/
	case 27:
		if(scene.GetLayout(LAYOUT_IN_GAME)->IsSet())
		{
			EventArg escapeHitEventArg[1];
			escapeHitEventArg[0].argType = "what_event";
			escapeHitEventArg[0].argument.varType = TYPE_STRING;
			strcpy(escapeHitEventArg[0].argument.varString, "pauseGame");
			Event escapeHitEvent = Event(1, EVENT_TYPE_OTHER, escapeHitEventArg);

			scene.OnEvent(escapeHitEvent);
		}
		break;
	case 32:
		isEmitterStarted = true;
		break;
	}
	/*
	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{
		Event returnedFusionEvent = StockEvents::EmptyEvent();
		scene.UpdateFusion(key, returnedFusionEvent);
		if(returnedFusionEvent.GetType() != StockEvents::EmptyEvent().GetType())
		{
			scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionOne")->OnEvent(returnedFusionEvent);
			scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionTwo")->OnEvent(returnedFusionEvent);
			scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionThree")->OnEvent(returnedFusionEvent);
			if(returnedFusionEvent.GetArgument("isHint").varBool == true)
			{
				EventArg showHintEventArgs[2];
				showHintEventArgs[0].argType = "what_event";
				showHintEventArgs[0].argument.varType = TYPE_STRING;
				strcpy(showHintEventArgs[0].argument.varString, "show_skill");
				showHintEventArgs[1].argType = "skillIndex";
				showHintEventArgs[1].argument.varType = TYPE_INTEGER;
				std::string currentInputSequence = scene.GetCurrentFusionInputSequence();
				showHintEventArgs[1].argument.varInteger = 
					scene.GetSkillTypeByFusionCombination(currentInputSequence[0], currentInputSequence[1], currentInputSequence[2]);
				Event showHintEvent(2, EVENT_TYPE_OTHER, showHintEventArgs);
				scene.GetLayout(LAYOUT_IN_GAME)->GetControl("skillHint")->OnEvent(showHintEvent);
			}
		}
	}*/

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}
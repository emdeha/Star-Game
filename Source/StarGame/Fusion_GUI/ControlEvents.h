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


#ifndef FE_CONTROL_EVENTS_H
#define FE_CONTROL_EVENTS_H


#include "../Fusion_EventManager/EntityEvents.h"


namespace FusionEngine 
{
	class Control;
}

void Click_NewGame(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_ResumeGame(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_SaveGame(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_LoadGame(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_Options(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_Back(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_Quit(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_TextBox(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void Click_Apply(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);

//void KeyPressed_TextBox(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);
void KeyPressed_FusionImageBox(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);

void KeyPressed_GameLayout(FusionEngine::Control *control, const FusionEngine::IEventData &eventData);


#endif
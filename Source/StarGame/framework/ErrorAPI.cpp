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


#include "ErrorAPI.h"


void HandleUnexpectedError(const std::string &message, unsigned line, const std::string &file,
						   Severity errorSeverity)
{
	printf("Error %s at line %u in file %s\n\n", message.c_str(), line, file.c_str());
	if (errorSeverity == FE_SEVERITY_HIGH)
	{
		_CrtDbgBreak();
	}
	else if (errorSeverity == FE_SEVERITY_MED)
	{
		printf("\a\a\a\a");
	}
}
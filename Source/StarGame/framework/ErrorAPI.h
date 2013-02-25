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


#ifndef ERROR_API_H
#define ERROR_API_H

#include <string>


// TODO: Make it work

#if defined (_DEBUG)
#define XASSERT(test, msg, ...) do { if (!(test)) error(__LINE__, __FILE__, \
			"Assertion failed: %s\n\n" msg, #test, __VA_ARGS__); } while(0)
#else
#define XASSERT(test, msg, ...) ((void)0)
#endif


void HandleUnexpectedError(const std::string &message, unsigned line, const std::string &file);


#endif
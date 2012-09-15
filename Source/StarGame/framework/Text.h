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


/// \ingroup module_StarGame

/// \file Text.h
/// \brief Displays text in an OpenGL window.

// TODO: Make a 3D text utility and a hovering text utility.
// TODO: Make the text formatable.

// The fontFace_bold shouldn't be here maybe. 
// To Note ^

#ifndef TEXT_H
#define TEXT_H


#include "Utility.h"

#include <ft2build.h>
#include FT_FREETYPE_H


/// \class Text
/// \brief Handles text displaying via FreeType2.

/// To show text you must first create an instance of the class with the
/// `fontName` you want. This `fontName` represents the font you want to render
/// the text with.
///
/// After initialization you can call Print to display the text on a chosen position 
/// on the screen.
///
/// UpdateWindowDimensions is used in order to scale the text relative to window's
/// width and height. This keeps the text with the same proportions regardless of 
/// window dimensions.
class Text
{
private:
	FT_Library ft; ///< Handle for the FreeType2 library
	FT_Face fontFace; ///< The font we will use for our text
	FT_Face fontFace_bold;

	int windowWidth;
	int windowHeight;

	float textMinWidth;
	float textMaxWidth;
	float textMinHeight;
	float textMaxHeight;

	GLuint vbo;
	GLuint vao;


	bool isBold;

public:
	Text();
	/// \fn Text
	/// \brief Initializes the FreeType2 library and sets the text's font
	Text(const char *fontName/*, const char *fontName_bold*/);

	/// \fn Init
	/// \brief Generates the OpenGL buffers needed for text drawing and sets `windowWidth` and `windowHeight`
	void Init(int newWindowWidth, int newWindowHeight);
	/// \fn UpdateWindowDimensions
	/// \brief Updates windowWidth and windowHeight
	void UpdateWindowDimensions(int newWindowWidth, int newWindowHeight);

	/// \fn Print
	/// \brief Prints the `text` we want to output to the screen.

	/// This function first creates a texture for all the `fontFace` glyphs.
	/// After that it goes through each character of `text` and draws it
	/// with its texture and corresponding alpha blending.
	///
	/// It doesn't make any text formatting.
	void Print(const char *text, const FontProgData &fontData,
			   glm::vec2 position,
			   glm::vec4 color, 
			   int fontSize);


	void ComputeTextDimensions(const char *text, glm::vec2 position, int fontSize);
	// returns the max corner of the character
	// the min corner is provided by the position parameter
	glm::vec2 ComputeCharacterDimensions(char ch, glm::vec2 position, int fontSize);

	void SetFont(bool newIsBold);

	bool IsBold();

	float GetTextMinWidth();
	float GetTextMaxWidth();
	float GetTextMinHeight();
	float GetTextMaxHeight();
};

inline void Text::SetFont(bool newIsBold)
{
	isBold = newIsBold;
}

inline bool Text::IsBold()
{
	return isBold;
}

inline float Text::GetTextMinWidth()
{
	return textMinWidth;
}
inline float Text::GetTextMaxWidth()
{
	return textMaxWidth;
}
inline float Text::GetTextMinHeight()
{
	return textMinHeight;
}
inline float Text::GetTextMaxHeight()
{
	return textMaxHeight;
}


#endif
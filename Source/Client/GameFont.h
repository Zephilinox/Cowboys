#pragma once

#include <vector>
/**
**   A loaded font. A font that's loaded in the game engine.
*/

struct LoadedGameFont
{
	LoadedGameFont(int idx, const char* n, int ptm);

	int id = 0;         /**< Font ID. The ID assigned to the font from the graphics engine. */
	int size = 0;       /**< The font size. The size of the font that was loaded. */
	const char* name;   /**< Name. The name of the font. */

	/**< Loaded Fonts. Cheap and nasty way of globalising loaded fonts. */
	static std::vector<LoadedGameFont> loaded_fonts; 
};


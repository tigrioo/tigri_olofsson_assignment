#pragma once
#include "raylib.h"

#include <string_view>

namespace res 
{
	Sound     get_or_load_sound(const char* file_name);
	Texture2D get_or_load_texture(const char* file_name);

	void unload();
}
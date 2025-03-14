#include "resources.h"
#include <iostream>

#include <unordered_map>

// struct is from "https://www.cppstories.com/2021/heterogeneous-access-cpp20/"
struct my_string_hash
{
    using is_transparent = void;

    [[nodiscard]] size_t operator()(const char *txt) const
	{
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]] size_t operator()(std::string_view txt) const
	{
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]] size_t operator()(const std::string &txt) const
	{
        return std::hash<std::string>{}(txt);
    }
};

namespace res 
{
	std::unordered_map<std::string, Sound, my_string_hash, std::equal_to<>>     loaded_sounds;
	std::unordered_map<std::string, Texture2D, my_string_hash, std::equal_to<>> loaded_textures;

	// The above are not strictly necessary, for simplicity you may use these:
	//
	// std::unordered_map<std::string, Sound> loaded_sounds;
	// std::unordered_map<std::string, Texture2D> loaded_textures;
	//
	// The difference is that with these, whenever you access them (e.g. using find or []), you
	// will create a temporary copy of the string. The version I have, which includes "my_string_hash", 
	// allows you to use "const char*, std::string_view and std::string" as keys without converting them to std::string
	// first. (Which is what causes the copy)


	Sound get_or_load_sound(const char* file_name)
	{
		auto loaded_sound = loaded_sounds.find(file_name);
		
		if(loaded_sound == loaded_sounds.end())
		{
			// The sound was not previously loaded, therefore we 
			// have to actually load it now.

			Sound new_sound = LoadSound(file_name);
			loaded_sound = loaded_sounds.insert({file_name, new_sound}).first;
		}

		return loaded_sound->second;
	}

	Texture2D get_or_load_texture(const char* file_name)
	{
		auto loaded_texture = loaded_textures.find(file_name);
		
		if(loaded_texture == loaded_textures.end())
		{
			Texture2D new_sound = LoadTexture(file_name);
			loaded_texture = loaded_textures.insert({file_name, new_sound}).first;
		}

		return loaded_texture->second;
	}

	void unload()
	{
		// Unload all sounds
		for(const auto& it : loaded_sounds)
		{
			UnloadSound(it.second);
		}
		loaded_sounds.clear();

		// Unload all textures
		for(const auto& it : loaded_textures)
		{
			UnloadTexture(it.second);
		}
		loaded_textures.clear();
	}
}
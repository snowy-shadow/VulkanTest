module;
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#include "EngineMacro.h"
export module VT.Function;

import VT.Log;

export namespace VT
{
	char* ToUTF8(const wchar_t const* Str)
	{
		#ifdef WIN32
		#endif
        return 0;
	}

	std::string ToUTF8(std::wstring Str)
	{ return {};
	}

	std::byte* LoadImage(const char* File, int* Width, int* Height, int* Channels, int DesiredChannels)
	{
        stbi_uc* Data = stbi_load(File, Width, Height, Channels, DesiredChannels);

        // Image failed to load, load default texture
        if (Data == nullptr)
        {
            VT_CORE_WARN("Could not load file : {0}. {1}\nLoading : {2}",
                         File,
                         stbi_failure_reason(), "Engine/Asset/DefaultTexture.png");
            Data = stbi_load("Engine/Asset/DefaultTexture.png", Width, Height, Channels, 0);
            VT_CORE_ASSERT(Data != nullptr, "Default Texture missing!");
        }

		return reinterpret_cast<std::byte*>(Data);
	}

	void UnloadImage(std::byte* Image)
    {
        stbi_image_free(Image);
        Image = nullptr;
    }
	
}
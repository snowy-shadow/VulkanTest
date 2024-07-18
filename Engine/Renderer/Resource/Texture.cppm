module;
#include "VT_Export"
#include <cstdint>
export module VT.Texture;

export namespace VT
{
	struct VT_ENGINE_EXPORT TextureCreateInfo
	{
        uint32_t ID;
        uint32_t Width;
        uint32_t Height;
        uint32_t ChannelCount;
        uint32_t Generation;
        uint32_t DataSize;
        void* pData;
        bool Transparent;
    };

    // TODO : fix interface
	struct VT_ENGINE_EXPORT Texture
	{
        uint32_t ID;
		uint32_t Width;
        uint32_t Height;
		uint32_t ChannelCount;
        uint32_t Generation;
        bool Transparent;
    };
}
#pragma once

#include <string>

namespace fxlang
{

enum class TextureType
{
    R8, 
    R16F, 
    R32F,
    RG8, 
    RG16, 
    RG16F, 
    RG32F,
    RGBA8, 
    RGBA16, 
    RGBA16F, 
    RGBA32F,
    RGB10A2
};

struct Texture
{
    std::string name;

    std::string filepath;

    int width  = 1;
    int height = 1;

    int mip_levels = 1;

    TextureType type = TextureType::RGBA8;
};

}
#pragma once

#include "fxlang/Uniform.h"
#include "fxlang/Texture.h"
#include "fxlang/Technique.h"

#include <vector>

namespace fxlang
{

struct Effect
{
    std::vector<std::string> includes;

    std::vector<Uniform>   uniforms;
    std::vector<Texture>   textures;
    std::vector<Technique> techniques;

    std::string functions;

}; // Effect

}
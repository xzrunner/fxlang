#pragma once

#include <string>

namespace glslang { class TShader; }

namespace fxlang
{

class ShaderParser
{
public:
	static glslang::TShader* 
		ParseHLSL(const std::string& shader);

}; // ShaderParser

}
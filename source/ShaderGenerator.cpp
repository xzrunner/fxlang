#include "fxlang/ShaderGenerator.h"
#include "fxlang/Effect.h"

#include <rttr/registration>

namespace
{

void str_replace(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty()) {
		return;
	}
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

}

namespace fxlang
{

ShaderGenerator::ShaderGenerator(const Effect& effect)
	: m_effect(effect)
{
}

void ShaderGenerator::ToHLSL(const Pass& pass, std::string& vs, 
	                         std::string& fs) const
{
}

std::string ShaderGenerator::ToHLSL() const
{
	auto funcs = m_effect.functions;
	str_replace(funcs, "ReShade::BackBuffer", "BackBuffer");

	std::string shader = R"(
#define BUFFER_WIDTH 800
#define BUFFER_HEIGHT 600

#define BUFFER_RCP_WIDTH (1.0/BUFFER_WIDTH)
#define BUFFER_RCP_HEIGHT (1.0/BUFFER_HEIGHT)

#define BUFFER_PIXEL_SIZE float2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT)
#define BUFFER_SCREEN_SIZE float2(BUFFER_WIDTH, BUFFER_HEIGHT)
#define BUFFER_ASPECT_RATIO (BUFFER_WIDTH * BUFFER_RCP_HEIGHT)

//namespace ReShade
//{
	// Global textures and samplers
	Texture2D BackBufferTex : register(t0);
	//Texture2D DepthBufferTex : register(t1);

	sampler2D BackBuffer : register(s0);
	//sampler2D DepthBuffer : register(s1);
//}
)";

	auto type_enum = rttr::type::get<VariableType>().get_enumeration();
	for (auto& unif : m_effect.uniforms)
	{
		std::string type_str = type_enum.value_to_name(unif.var.type).to_string();
		shader += "uniform " + type_str + " " + unif.var.name + ";\n";
	}

	shader += funcs;

	return shader;
}

}
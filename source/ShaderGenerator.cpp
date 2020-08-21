#include "fxlang/ShaderGenerator.h"
#include "fxlang/Effect.h"

#include <rttr/registration>

#include <assert.h>
#include <stdarg.h>

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

std::string str_format(const std::string fmt, ...)
{
	int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
	std::string str;
	va_list ap;
	while (1) {     // Maximum two passes on a POSIX system...
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char*)str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {  // Everything worked
			str.resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

}

namespace fxlang
{

ShaderGenerator::ShaderGenerator(const Effect& effect)
	: m_effect(effect)
{
}

//void ShaderGenerator::ToHLSL(const Pass& pass, std::string& vs, 
//	                         std::string& fs) const
//{
//}

void ShaderGenerator::Gen(std::string& vs, std::string& fs,
	                      int tech_idx, int pass_idx) const
{
	assert(tech_idx == 0 && pass_idx == 0);
	auto& pass = m_effect.techniques[tech_idx].passes[pass_idx];

	switch (m_effect.type)
	{
	case EffectType::Default:
		break;
	case EffectType::DX_CG:
		break;
	case EffectType::Reshade:
	{
		assert(pass.vertex_shader == "PostProcessVS");
		vs = R"(
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// Vertex shader generating a triangle covering the entire screen
void PostProcessVS(in uint id : SV_VertexID, out float4 position : SV_Position, out float2 texcoord : TEXCOORD)
{
	texcoord.x = (id == 2) ? 2.0 : 0.0;
	texcoord.y = (id == 1) ? 2.0 : 0.0;
	position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
}

VS_OUTPUT main(float3 pos : POSITION, float2 texcoord : TEXCOORD)
{
    VS_OUTPUT vertex_output;

	//PostProcessVS(0, vertex_output.pos, vertex_output.texcoord);

	vertex_output.pos = float4(pos, 1.0);
	vertex_output.texcoord = texcoord;

	return vertex_output;
}
)";

		auto funcs = m_effect.functions;
		//str_replace(funcs, "ReShade::BackBuffer", "BackBuffer");
		str_replace(funcs, "tex2D(ReShade::BackBuffer", "BackBufferTex.Sample(BackBuffer");

		fs = R"(
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

		uniform sampler BackBuffer : register(s0);
		//sampler2D DepthBuffer : register(s1);
	//}
	)";

		auto type_enum = rttr::type::get<VariableType>().get_enumeration();
		for (auto& unif : m_effect.uniforms)
		{
			std::string type_str = VarTypeToString(unif.var.type, false);
			fs += "uniform " + type_str + " " + unif.var.name + ";\n";
		}

		fs += funcs;

		fs += str_format(R"(
float4 main(float4 pos : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return float4(%s(pos, texcoord), 1.0);
}
)", pass.pixel_shader.c_str());
	}
		break;
	}
}

std::string ShaderGenerator::VarTypeToString(VariableType type, bool glsl)
{
	if (glsl)
	{
		switch (type)
		{
		case VariableType::Int:
			return "int";
		case VariableType::Bool:
			return "bool";
		case VariableType::Float:
			return "float";
		case VariableType::Float2:
			return "vec2";
		case VariableType::Float3:
			return "vec3";
		case VariableType::Float4:
			return "vec4";
		default:
			assert(0);
		}
	}
	else
	{
		switch (type)
		{
		case VariableType::Int:
			return "int";
		case VariableType::Bool:
			return "bool";
		case VariableType::Float:
			return "float";
		case VariableType::Float2:
			return "float2";
		case VariableType::Float3:
			return "float3";
		case VariableType::Float4:
			return "float4";
		default:
			assert(0);
		}
	}
	return "";
}

}
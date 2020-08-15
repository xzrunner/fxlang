#include "fxlang/EffectBuilder.h"
#include "fxlang/Effect.h"

#include <rttr/registration>

#include <sstream>

namespace fxlang
{

EffectBuilder::EffectBuilder(const Effect& effect)
	: m_effect(effect)
{
}

std::string EffectBuilder::GenCode() const
{
	std::stringstream ss;
	GenIncludes(ss);
	ss << "\n";
	GenUniforms(ss);
	ss << "\n";
	GenTechniques(ss);

	return ss.str();
}

void EffectBuilder::GenIncludes(std::ostream& out) const
{
	for (auto& path : m_effect.includes) {
		out << "include \"" + path + "\"\n";
	}
}

void EffectBuilder::GenUniforms(std::ostream& out) const
{
	auto type_enum = rttr::type::get<VariableType>().get_enumeration();
	for (auto& unif : m_effect.uniforms)
	{
		std::string type_str = type_enum.value_to_name(unif.var.type).to_string();
		out << "uniform " + type_str + " " + unif.var.name + ";\n";
	}
}

void EffectBuilder::GenTechniques(std::ostream& out) const
{
	for (auto& tech : m_effect.techniques)
	{
		out << "technique " << tech.name << "\n";
		out << "{\n";
		for (auto& pass : tech.passes) {
			GenPass(out, pass);
		}
		out << "}\n";
	}
}

void EffectBuilder::GenPass(std::ostream& out, const Pass& pass) const
{
	out << "pass " << pass.name << "\n";
	out << "{\n";
	out << "VertexShader = " << pass.vertex_shader << "\n";
	out << "PixelShader = " << pass.pixel_shader << "\n";
	out << "}\n";
}

}
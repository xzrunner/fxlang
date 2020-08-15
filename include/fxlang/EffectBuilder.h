#pragma once

#include <string>

namespace fxlang
{

struct Effect;
struct Pass;

class EffectBuilder
{
public:
	EffectBuilder(const Effect& effect);

	std::string GenCode() const;

private:
	void GenIncludes(std::ostream& out) const;
	void GenUniforms(std::ostream& out) const;
	void GenTechniques(std::ostream& out) const;
	void GenPass(std::ostream& out, const Pass& pass) const;

private:
	const Effect& m_effect;

}; // EffectBuilder

}
#pragma once

#include <string>

namespace fxlang
{

struct Effect;
struct Pass;

class ShaderGenerator
{
public:
	ShaderGenerator(const Effect& effect);

	void ToHLSL(const Pass& pass, 
		std::string& vs, std::string& fs) const;

	std::string ToHLSL() const;

private:
	const Effect& m_effect;

}; // ShaderGenerator

}
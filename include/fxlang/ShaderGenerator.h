#pragma once

#include "fxlang/VariableType.h"

#include <string>

namespace fxlang
{

struct Effect;
struct Pass;

class ShaderGenerator
{
public:
	ShaderGenerator(const Effect& effect);

	//void ToHLSL(const Pass& pass, 
	//	std::string& vs, std::string& fs) const;

	void Gen(std::string& vs, std::string& fs, 
		int tech_idx = 0, int pass_idx = 0) const;

private:
	void GenReshade(const Pass& pass,
		std::string& vs, std::string& fs) const;

	static std::string VarTypeToString(VariableType type, bool glsl = true);

private:
	const Effect& m_effect;

}; // ShaderGenerator

}
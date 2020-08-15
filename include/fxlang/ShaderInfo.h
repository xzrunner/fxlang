#pragma once

#include <string>
#include <vector>

class TIntermNode;
namespace glslang  { class TIntermAggregate; }

namespace fxlang
{

class ShaderInfo
{
public:
	static glslang::TIntermAggregate* 
		GetFunctionByName(const std::string& name, TIntermNode& root) noexcept;

	struct FunctionParameter {
		enum Qualifier { IN, OUT, INOUT, CONST };
		std::string name;
		std::string type;
		Qualifier qualifier;
	};
	static void GetFunctionParameters(glslang::TIntermAggregate* func, 
		std::vector<FunctionParameter>& output) noexcept;

}; // ShaderInfo

}
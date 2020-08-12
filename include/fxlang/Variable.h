#pragma once

#include "fxlang/VariableType.h"

#include <string>

namespace fxlang
{

struct Variable
{
	VariableType type;
	std::string  name;

	union {
		int   i;
		bool  b;
		float f;
		float f2[2];
		float f3[3];
		float f4[4];
	};

}; // Variable

}
#include "fxlang/VariableType.h"

#include <rttr/registration>

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::enumeration<fxlang::VariableType>("fxlang_var_type")
(
    REGIST_ENUM_ITEM(fxlang::VariableType::Int,    "int",    "Int"),
    REGIST_ENUM_ITEM(fxlang::VariableType::Bool,   "bool",   "Bool"),
    REGIST_ENUM_ITEM(fxlang::VariableType::Float,  "float",  "Float"),
    REGIST_ENUM_ITEM(fxlang::VariableType::Float2, "float2", "Float2"),
    REGIST_ENUM_ITEM(fxlang::VariableType::Float3, "float3", "Float3"),
    REGIST_ENUM_ITEM(fxlang::VariableType::Float4, "float4", "Float4")
);

}

namespace fxlang
{

void regist_rttr()
{
}

}
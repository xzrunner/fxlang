#pragma once

#include "fxlang/Variable.h"

namespace fxlang
{

struct Uniform
{
    Variable var;

    std::string ui_type;

    float ui_min = 0;
    float ui_max = 0;

    std::string ui_items;
    std::string ui_label;
    std::string ui_tooltip;
};

}
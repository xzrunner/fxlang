#pragma once

#include <string>

namespace fxlang
{

enum class TexAddress
{
    Clamp, 
    Mirror, 
    wrap,
    Repeat, 
    Border
};

enum class TexFilter
{
    Point, 
    Linear
};

struct Sampler
{
    std::string texture;

	TexAddress address_u = TexAddress::Clamp;
	TexAddress address_v = TexAddress::Clamp;
	TexAddress address_w = TexAddress::Clamp;

	TexFilter mag_filter = TexFilter::Linear;
	TexFilter min_filter = TexFilter::Linear;
	TexFilter mip_filter = TexFilter::Linear;

	float min_lod = 0.0f;
	float max_lod = 1000.0f;

	float mip_lod_bias = 0.0f;

	bool srgb_texture = false;
};

}
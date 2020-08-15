#pragma once

#include <string>
#include <vector>

namespace fxlang
{

enum class PrimitiveTopology
{
	PointList, 
	LineList, 
	LineStrip, 
	TriangleList, 
	TriangleStrip
};

enum class BlendEquation
{
	Add, 
	Subtract, 
	Revsubtract, 
	Min, 
	Max
};

enum class BlendingFactor
{
	Zero, 
	One,
	SrcColor, 
	SrcAlpha, 
	InvSrcColor, 
	InvSrcAlpha,
	DestColor, 
	DestAlpha, 
	InvDestColor, 
	InvDestAlpha
};

enum class StencilTestFunc
{
	Never, 
	Always,
	Equal, 
	NotEqual,
	Less,
	Greater, 
	LessEqual, 
	GreaterEqual
};

enum class StencilOperation
{
	Keep, 
	Zero, 
	Replace, 
	Incr, 
	Incrsat, 
	Decr, 
	Decrsat, 
	Invert
};

struct Pass
{
	std::string name;

	PrimitiveTopology prim_type;

	unsigned int vertex_count = 0;

	std::string vertex_shader;
	std::string pixel_shader;

	std::string render_target;

	bool clear_render_targets = false;

	unsigned int render_target_write_mask = 0;

	bool srgb_write_enable = false;

	bool blend_enable = false;

	BlendEquation blend_op       = BlendEquation::Add;
	BlendEquation blend_op_alpha = BlendEquation::Add;

	BlendingFactor src_blend        = BlendingFactor::One;
	BlendingFactor src_blend_alpha  = BlendingFactor::One;
	BlendingFactor dest_blend       = BlendingFactor::Zero;
	BlendingFactor dest_blend_alpha = BlendingFactor::Zero;

	bool stencil_enable = false;

	unsigned char stencil_read_mask  = 0xff;
	unsigned char stencil_write_mask = 0xff;

	StencilTestFunc stencil_func = StencilTestFunc::Always;

	int stencil_ref = 0;

	StencilOperation stencil_pass_op       = StencilOperation::Keep;
	StencilOperation stencil_fail_op       = StencilOperation::Keep;
	StencilOperation stencil_depth_fail_op = StencilOperation::Keep;
};

struct Technique
{
	std::string name;

	// annotations
	bool enabled = true;
	int timeout = 0;
	int toggle = 0;
	bool togglectrl = false;
	bool toggleshift = false;
	bool togglealt = false;
	bool hidden = false;
	std::string ui_tooltip;

	std::vector<Pass> passes;
};

}
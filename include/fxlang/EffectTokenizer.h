#pragma once

#include <lexer/Tokenizer.h>

namespace fxlang
{

namespace Token
{
	typedef unsigned int Type;

	static const Type Integer       = 1 <<  0; // integer number
	static const Type Decimal       = 1 <<  1; // decimal number
	static const Type String        = 1 <<  2; // string
    static const Type Comment       = 1 <<  3; // line comment starting with ///

	static const Type OParenthesis  = 1 <<  4; // opening parenthesis: (
	static const Type CParenthesis  = 1 <<  5; // closing parenthesis: )
	static const Type OBrace        = 1 <<  6; // opening brace: {
	static const Type CBrace        = 1 <<  7; // closing brace: }
	static const Type OBracket      = 1 <<  8; // opening bracket: [
	static const Type CBracket      = 1 <<  9; // closing bracket: ]
	static const Type OAngleBracket = 1 << 10; // opening angle brackets: <
	static const Type CAngleBracket = 1 << 11; // closing angle brackets: >

    static const Type Comma         = 1 << 12; // ,
    static const Type Dot           = 1 << 13; // .
    static const Type Pound         = 1 << 14; // #
    static const Type Colon         = 1 << 15; // :
    static const Type Question      = 1 << 16; // ?

    static const Type Add           = 1 << 17; // +
    static const Type Sub           = 1 << 18; // _
    static const Type Mul           = 1 << 19; // *
    static const Type Div           = 1 << 20; // /
    static const Type Equal         = 1 << 21; // =

	static const Type Eof           = 1 << 22; // end of file
	static const Type Eol           = 1 << 23; // end of line
}

class EffectTokenizer : public lexer::Tokenizer<Token::Type>
{
public:
    EffectTokenizer(const std::string& str);

protected:
    virtual Token EmitToken() override;

    static const std::string& NumberDelim();

}; // EffectTokenizer

}
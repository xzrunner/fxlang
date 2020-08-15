#pragma once

#include "fxlang/EffectTokenizer.h"
#include "fxlang/Variable.h"
#include "fxlang/Effect.h"

#include <lexer/Parser.h>

namespace fxlang
{

class EffectParser : public lexer::Parser<fxlang::Token::Type>
{
public:
    EffectParser(const std::string& str);

    void Parse();

    auto& GetEffect() const { return m_effect; }

private:
    virtual std::map<fxlang::Token::Type, std::string> TokenNames() const {
        return std::map<fxlang::Token::Type, std::string>();
    }

private:
    void ParseUniform();
    void ParseTexture();
    void ParseTechnique();
    void ParsePass(Technique& tech);

    Variable ParseVariable() const;
    bool ParseBool() const;

    void FlushFuncCode();

private:
    mutable EffectTokenizer m_tokenizer;

    Effect m_effect;

    const char* m_func_last_pos = nullptr;

    //typedef EffectTokenizer::Token Token;

}; // EffectParser

}
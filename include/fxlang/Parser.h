#pragma once

#include "fxlang/Tokenizer.h"
#include "fxlang/Variable.h"
#include "fxlang/Uniform.h"
#include "fxlang/Texture.h"

#include <lexer/Parser.h>

namespace fxlang
{

class Parser : public lexer::Parser<fxlang::Token::Type>
{
public:
    Parser(const std::string& str);

    void Parse();

    auto& GetIncludes() const { return m_includes; }
    auto& GetUniforms() const { return m_uniforms; }
    auto& GetTextures() const { return m_textures; }

private:
    virtual std::map<fxlang::Token::Type, std::string> TokenNames() const {
        return std::map<fxlang::Token::Type, std::string>();
    }

private:
    void ParseUniform();
    void ParseTexture();
    void ParseTechnique();

    Variable ParseVariable() const;

private:
    mutable Tokenizer m_tokenizer;

    std::vector<std::string> m_includes;

    std::vector<Uniform> m_uniforms;
    std::vector<Texture> m_textures;

    //typedef Tokenizer::Token Token;

}; // Parser

}
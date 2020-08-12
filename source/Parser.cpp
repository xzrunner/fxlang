#include "fxlang/Parser.h"

#include <exception>

namespace fxlang
{

Parser::Parser(const std::string& str)
    : m_tokenizer(Tokenizer(str))
{
}

void Parser::Parse()
{
    Tokenizer::Token token = m_tokenizer.PeekToken();
    fxlang::Token::Type token_type = token.GetType();
    while (token_type != fxlang::Token::Eof)
    {
        bool advanced = false;
        switch (token_type)
        {
        case fxlang::Token::String:
        {
            auto str = token.Data();
            if (str == "uniform") {
                ParseUniform();
                advanced = true;
            } else if (str == "texture") {
                ParseTexture();
                advanced = true;
            } else if (str == "technique") {
                ParseTechnique();
                advanced = true;
            }
        }
            break;
        case fxlang::Token::Pound:
        {
            Expect(fxlang::Token::Pound, token = m_tokenizer.NextToken());
            auto str = token.Data();
            if (strncmp(str.c_str(), "#include", strlen("include")) == 0) 
            {
                auto filepath = str.substr(strlen("include") + 3);
                filepath.pop_back();
                m_includes.push_back(filepath);
            }
            advanced = true;
        }
            break;
        }

        if (!advanced) {
            m_tokenizer.NextToken();
        }

        token = m_tokenizer.PeekToken();
        token_type = token.GetType();
    }
}

void Parser::ParseUniform()
{
    Uniform uniform;

    Tokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "uniform");
    uniform.var = ParseVariable();

    if (m_tokenizer.NextToken().GetType() == fxlang::Token::OAngleBracket)
    {
        while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CAngleBracket)
        {
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            auto prop = token.Data();
            if (prop == "ui_type") 
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                uniform.ui_type = token.Data();
            } 
            else if (prop == "ui_min")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                token = m_tokenizer.NextToken();
                if (!Check(fxlang::Token::Integer, token) && !Check(fxlang::Token::Decimal, token)) {
                    assert(0);
                    throw lexer::ParserException(token.Line(), token.Column(), "number");
                }
                uniform.ui_min = static_cast<float>(std::atof(token.Data().c_str()));
            }
            else if (prop == "ui_max")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                token = m_tokenizer.NextToken();
                if (!Check(fxlang::Token::Integer, token) && !Check(fxlang::Token::Decimal, token)) {
                    assert(0);
                    throw lexer::ParserException(token.Line(), token.Column(), "number");
                }
                uniform.ui_max = static_cast<float>(std::atof(token.Data().c_str()));
            }
            else if (prop == "ui_items") 
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                uniform.ui_items = token.Data();
            }
            else if (prop == "ui_label")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                uniform.ui_label = token.Data();
            }
            else if (prop == "ui_tooltip")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                uniform.ui_tooltip = token.Data();
            }
            else if (prop == "ui_step" || prop == "ui_label" || prop == "ui_category "
                  || prop == "ui_category_closed " || prop == "ui_spacing ")
            {
                assert(0);
                throw std::runtime_error("Unknown type!");
            }
            else
            {
                // skip __UNIFORM_SLIDER_FLOAT1
            }
        }
        token = m_tokenizer.NextToken(); // skip CAngleBracket
    }

    Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());

    switch (uniform.var.type)
    {
    case VariableType::Int:
        Expect(fxlang::Token::Integer, token = m_tokenizer.NextToken());
        uniform.var.i = std::atoi(token.Data().c_str());
        break;
    case VariableType::Bool:
    {
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        auto str = token.Data();
        if (str == "true") {
            uniform.var.b = true;
        } else if (str == "false") {
            uniform.var.b = false;
        } else {
            assert(0);
            throw std::runtime_error("Unknown type!");
        }
    }
        break;
    case VariableType::Float:
        Expect(fxlang::Token::Decimal, token = m_tokenizer.NextToken());
        uniform.var.f = static_cast<float>(std::atof(token.Data().c_str()));
        break;
    case VariableType::Float2:
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        assert(token.Data() == "float2");
        Expect(fxlang::Token::OParenthesis, token = m_tokenizer.NextToken());
        for (int i = 0; i < 2; ++i) {
            Expect(fxlang::Token::Decimal, token = m_tokenizer.NextToken());
            uniform.var.f2[i] = static_cast<float>(std::atof(token.Data().c_str()));
        }
        Expect(fxlang::Token::CParenthesis, token = m_tokenizer.NextToken());
        break;
    case VariableType::Float3:
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        assert(token.Data() == "float3");
        Expect(fxlang::Token::OParenthesis, token = m_tokenizer.NextToken());
        for (int i = 0; i < 3; ++i) {
            Expect(fxlang::Token::Decimal, token = m_tokenizer.NextToken());
            uniform.var.f3[i] = static_cast<float>(std::atof(token.Data().c_str()));
        }
        Expect(fxlang::Token::CParenthesis, token = m_tokenizer.NextToken());
        break;
    case VariableType::Float4:
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        assert(token.Data() == "float4");
        Expect(fxlang::Token::OParenthesis, token = m_tokenizer.NextToken());
        for (int i = 0; i < 4; ++i) {
            Expect(fxlang::Token::Decimal, token = m_tokenizer.NextToken());
            uniform.var.f4[i] = static_cast<float>(std::atof(token.Data().c_str()));
        }
        Expect(fxlang::Token::CParenthesis, token = m_tokenizer.NextToken());
        break;
    }

    m_uniforms.push_back(uniform);
}

void Parser::ParseTexture()
{
    Texture tex;

    Tokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "texture");

    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    tex.name = token.Data();

    if (m_tokenizer.NextToken().GetType() == fxlang::Token::OAngleBracket)
    {
        while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CAngleBracket)
        {
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            auto prop = token.Data();
            if (prop == "source") 
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                tex.filepath = token.Data();
            } 
            else
            {
                assert(0);
                throw std::runtime_error("Unknown type!");
            }
        }
        token = m_tokenizer.NextToken(); // skip CAngleBracket
    }

    Expect(fxlang::Token::OBrace, token = m_tokenizer.NextToken());
    while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CBrace)
    {
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        auto prop = token.Data();
        if (prop == "Width")
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            //tex.width = token.Data();
        }
        else if (prop == "Height")
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            //tex.height = token.Data();
        }
        else if (prop == "MipLevels")
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::Integer, token = m_tokenizer.NextToken());
            tex.mip_levels = std::atoi(token.Data().c_str());
        }
        else if (prop == "Format")
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            auto str = token.Data();
            if (str == "R8") {
                tex.type = TextureType::R8;
            } else if (str == "R16F") {
                tex.type = TextureType::R16F;
            } else if (str == "R32F") {
                tex.type = TextureType::R32F;
            } else if (str == "RG8") {
                tex.type = TextureType::RG8;
            } else if (str == "RG16") {
                tex.type = TextureType::RG16;
            } else if (str == "RG16F") {
                tex.type = TextureType::RG16F;
            } else if (str == "RG32F") {
                tex.type = TextureType::RG32F;
            } else if (str == "RGBA8") {
                tex.type = TextureType::RGBA8;
            } else if (str == "RGBA16") {
                tex.type = TextureType::RGBA16;
            } else if (str == "RGBA16F") {
                tex.type = TextureType::RGBA16F;
            } else if (str == "RGBA32F") {
                tex.type = TextureType::RGBA32F;
            } else if (str == "RGB10A2") {
                tex.type = TextureType::RGB10A2;
            } else {
                assert(0);
                throw std::runtime_error("Unknown type!");
            }
        }
        else
        {
            assert(0);
            throw std::runtime_error("Unknown type!");
        }
    }

    m_textures.push_back(tex);
}

Variable Parser::ParseVariable() const
{
    Variable ret;

    Tokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    auto type_str = token.Data();
    if (type_str == "int") {
        ret.type = VariableType::Int;
    } else if (type_str == "bool") {
        ret.type = VariableType::Bool;
    } else if (type_str == "float") {
        ret.type = VariableType::Float;
    } else if (type_str == "float2") {
        ret.type = VariableType::Float2;
    } else if (type_str == "float3") {
        ret.type = VariableType::Float3;
    } else if (type_str == "float4") {
        ret.type = VariableType::Float4;
    } else {
        assert(0);
        throw std::runtime_error("Unknown var type: %s!" + type_str);
    }

    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    ret.name = token.Data();
  
    return ret;
}

}
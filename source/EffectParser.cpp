#include "fxlang/EffectParser.h"

#include <exception>

namespace fxlang
{

EffectParser::EffectParser(const std::string& str, EffectType type)
    : m_tokenizer(EffectTokenizer(str))
{
    m_func_last_pos = m_tokenizer.Begin();

    m_effect.type = type;
}

void EffectParser::Parse()
{
    EffectTokenizer::Token token = m_tokenizer.PeekToken();
    fxlang::Token::Type token_type = token.GetType();
    while (token_type != fxlang::Token::Eof)
    {
        bool advanced = false;
        switch (token_type)
        {
        case fxlang::Token::String:
        {
            auto str = token.Data();
            if (str == "uniform") 
            {
                FlushFuncCode();
                ParseUniform();
                advanced = true;
                m_func_last_pos = m_tokenizer.CurPos();
            } 
            else if (str == "texture") 
            {
                FlushFuncCode();
                ParseTexture();
                advanced = true;
                m_func_last_pos = m_tokenizer.CurPos();
            } 
            else if (str == "technique") 
            {
                FlushFuncCode();
                ParseTechnique();
                advanced = true;
                m_func_last_pos = m_tokenizer.CurPos();
            }
        }
            break;
        case fxlang::Token::Pound:
        {
            FlushFuncCode();
            Expect(fxlang::Token::Pound, token = m_tokenizer.NextToken());
            auto str = token.Data();
            if (strncmp(str.c_str(), "#include", strlen("include")) == 0) 
            {
                auto filepath = str.substr(strlen("include") + 3);
                filepath.pop_back();
                m_effect.includes.push_back(filepath);

                m_func_last_pos = m_tokenizer.CurPos();
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

    m_effect.functions.append(m_func_last_pos, m_tokenizer.End());

    //printf("%s\n", m_functions.c_str());
}

void EffectParser::ParseUniform()
{
    Uniform uniform;

    EffectTokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "uniform");
    uniform.var = ParseVariable();

    if (m_tokenizer.PeekToken().GetType() == fxlang::Token::OAngleBracket)
    {
        m_tokenizer.NextToken();   // skip <
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
        uniform.var.b = ParseBool();
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

    m_effect.uniforms.push_back(uniform);
}

void EffectParser::ParseTexture()
{
    Texture tex;

    EffectTokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "texture");

    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    tex.name = token.Data();

    if (m_tokenizer.PeekToken().GetType() == fxlang::Token::OAngleBracket)
    {
        m_tokenizer.NextToken();   // skip <
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

    m_effect.textures.push_back(tex);
}

void EffectParser::ParseTechnique()
{
    Technique tech;

    EffectTokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "technique");

    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    tech.name = token.Data();

    if (m_tokenizer.PeekToken().GetType() == fxlang::Token::OAngleBracket)
    {
        m_tokenizer.NextToken();   // skip <
        while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CAngleBracket)
        {
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            auto prop = token.Data();
            if (prop == "enabled") 
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                tech.enabled = ParseBool();
            } 
            else if (prop == "timeout")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::Integer, token = m_tokenizer.NextToken());
                tech.timeout = std::atoi(token.Data().c_str());
            }
            else if (prop == "toggle")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::Integer, token = m_tokenizer.NextToken());
                tech.toggle = std::atoi(token.Data().c_str());
            }
            else if (prop == "togglectrl")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                tech.togglectrl = ParseBool();
            }
            else if (prop == "toggleshift")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                tech.toggleshift = ParseBool();
            }
            else if (prop == "togglealt")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                tech.togglealt = ParseBool();
            }
            else if (prop == "hidden")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                tech.hidden = ParseBool();
            }
            else if (prop == "ui_tooltip")
            {
                Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
                Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
                tech.ui_tooltip = token.Data();
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
    while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CBrace) {
        ParsePass(tech);
    }
    m_tokenizer.NextToken(); // skip CBrace

    m_effect.techniques.push_back(tech);
}

void EffectParser::ParsePass(Technique& tech)
{
    Pass pass;

    EffectTokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    assert(token.Data() == "pass");

    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    pass.name = token.Data();

    Expect(fxlang::Token::OBrace, token = m_tokenizer.NextToken());
    while (m_tokenizer.PeekToken().GetType() != fxlang::Token::CBrace) 
    {
        Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
        auto prop = token.Data();
        if (prop == "VertexShader") 
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            pass.vertex_shader = token.Data();
        } 
        else if (prop == "PixelShader")
        {
            Expect(fxlang::Token::Equal, token = m_tokenizer.NextToken());
            Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
            pass.pixel_shader = token.Data();
        }
        else
        {
            assert(0);
            throw std::runtime_error("Unknown type!");
        }
    }
    token = m_tokenizer.NextToken(); // skip CBrace

    tech.passes.push_back(pass);
}

Variable EffectParser::ParseVariable() const
{
    Variable ret;

    EffectTokenizer::Token token;
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

bool EffectParser::ParseBool() const
{
    EffectTokenizer::Token token;
    Expect(fxlang::Token::String, token = m_tokenizer.NextToken());
    auto str = token.Data();
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else {
        assert(0);
        throw std::runtime_error("Parse bool fail!");
    }
}

void EffectParser::FlushFuncCode()
{
    auto curr = m_tokenizer.CurPos();
    if (curr == m_func_last_pos) {
        return;
    }

    m_effect.functions.append(m_func_last_pos, curr - m_func_last_pos + 1);
    m_func_last_pos = curr;
}

}
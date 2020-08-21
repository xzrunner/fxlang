#include "fxlang/EffectTokenizer.h"

#include <lexer/Exception.h>

namespace fxlang
{

EffectTokenizer::EffectTokenizer(const std::string& str)
    : lexer::Tokenizer<fxlang::Token::Type>(str.c_str(), str.c_str() + str.length(), "\"", '\\')
{
}

lexer::Tokenizer<fxlang::Token::Type>::Token
EffectTokenizer::EmitToken()
{
    while (!Eof())
	{
        size_t start_line   = Line();
        size_t start_column = Column();
        const char* c = CurPos();
        switch (*c)
		{
            case '/':
                Advance();
                if (CurChar() == '/')
				{
                    const char* e = DiscardUntil("\n\r");
                    return Token(fxlang::Token::Comment, c, e, Offset(c), start_line, start_column);
                }
                else
                {
                    return Token(fxlang::Token::Div, c, c + 1, Offset(c), start_line, start_column);
                }
                break;
            case '{':
                Advance();
                return Token(fxlang::Token::OBrace, c, c+1, Offset(c), start_line, start_column);
            case '}':
                Advance();
                return Token(fxlang::Token::CBrace, c, c+1, Offset(c), start_line, start_column);
            case '(':
                Advance();
                return Token(fxlang::Token::OParenthesis, c, c+1, Offset(c), start_line, start_column);
            case ')':
                Advance();
                return Token(fxlang::Token::CParenthesis, c, c+1, Offset(c), start_line, start_column);
            case '[':
                Advance();
                return Token(fxlang::Token::OBracket, c, c+1, Offset(c), start_line, start_column);
            case ']':
                Advance();
                return Token(fxlang::Token::CBracket, c, c+1, Offset(c), start_line, start_column);
            case '<':
                Advance();
                return Token(fxlang::Token::OAngleBracket, c, c + 1, Offset(c), start_line, start_column);
            case '>':
                Advance();
                return Token(fxlang::Token::CAngleBracket, c, c + 1, Offset(c), start_line, start_column);
            case ',':
                Advance();
                return Token(fxlang::Token::Comma, c, c + 1, Offset(c), start_line, start_column);
            case '.':
                Advance();
                return Token(fxlang::Token::Dot, c, c + 1, Offset(c), start_line, start_column);
            case '#':
            {
                const char* e = DiscardUntil("\n\r");
                return Token(fxlang::Token::Pound, c, e, Offset(c), start_line, start_column);
            }
            case ':':
                Advance();
                return Token(fxlang::Token::Colon, c, c + 1, Offset(c), start_line, start_column);
            case '?':
                Advance();
                return Token(fxlang::Token::Question, c, c + 1, Offset(c), start_line, start_column);
            case '+':
                Advance();
                return Token(fxlang::Token::Add, c, c + 1, Offset(c), start_line, start_column);
            case '-':
            {
                const char* e = ReadInteger(NumberDelim());
                if (e != nullptr) {
                    return Token(fxlang::Token::Integer, c, e, Offset(c), start_line, start_column);
                }

                e = ReadDecimal(NumberDelim());
                if (e != nullptr) {
                    return Token(fxlang::Token::Decimal, c, e, Offset(c), start_line, start_column);
                }

                Advance();
                return Token(fxlang::Token::Sub, c, c + 1, Offset(c), start_line, start_column);
            }
            case '*':
                Advance();
                return Token(fxlang::Token::Mul, c, c + 1, Offset(c), start_line, start_column);
            case '=':
                Advance();
                return Token(fxlang::Token::Equal, c, c + 1, Offset(c), start_line, start_column);
            case '"': { // quoted string
                Advance();
                c = CurPos();
                const char* e = ReadQuotedString('"', "\n}");
                return Token(fxlang::Token::String, c, e, Offset(c), start_line, start_column);
            }
            case '\n':
                //if (!m_skip_eol) {
                //    Advance();
                //    return Token(fxlang::Token::Eol, c, c+1, Offset(c), start_line, start_column);
                //}
//                SwitchFallthrough();
            case '\r':
            case ' ':
            case '\t':
            case ';':
            {
                static const std::string whitespace(Whitespace() + ";");
                DiscardWhile(whitespace);
            }
                break;
            default: { // whitespace, integer, decimal or word
                const char* e = ReadInteger(NumberDelim());
				if (e != nullptr) {
					return Token(fxlang::Token::Integer, c, e, Offset(c), start_line, start_column);
				}

                e = ReadDecimal(NumberDelim());
				if (e != nullptr) {
					return Token(fxlang::Token::Decimal, c, e, Offset(c), start_line, start_column);
				}

                static const std::string str_separator(Whitespace() + ";{}[](),.#:?+-*/=");
                e = ReadUntil(str_separator);
				if (e == nullptr) {
                    assert(0);
					throw lexer::ParserException(start_line, start_column, "Unexpected character: " + std::string(c, 1));
				}

                return Token(fxlang::Token::String, c, e, Offset(c), start_line, start_column);
            }
        }
    }
    return Token(fxlang::Token::Eof, nullptr, nullptr, Length(), Line(), Column());
}

const std::string& EffectTokenizer::NumberDelim()
{
    static const std::string number_delim(Whitespace() + ")]};,");
    return number_delim;
}

}
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"
#include "Util.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR",
    "DECINT", "BININT", "HEXINT", "OCTINT",
    "WYSIWYGSTR", "DOUBLESTR", "DELIMITEDSTR", "TOKENSTR",
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    input.stream = &std::cin;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    input.GetChar(c);
    tmp.line_no = line_no;
    while (!input.EndOfInput() && !isspace(c) && !isDelimiterOfToken(c)) {
        tmp.lexeme += c;
        input.GetChar(c);
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    if (tmp.lexeme.length()<=1 && isdigit(tmp.lexeme[0])) {
        tmp.token_type = DECINT;
        return tmp;
    }
    else if (tmp.lexeme[0]=='0') {
        int number_prefix=1;
        if (toupper(tmp.lexeme[1])=='X') {
            ++number_prefix;
            int digit_encountered=0;
            for (char c: tmp.lexeme.substr(2)) {
                if (isxdigit(c)) {
                    digit_encountered=1;
                    ++number_prefix;
                }
                else if (c=='_') {
                    ++number_prefix;
                }
                else break;
            }
            int valid_integer_suffix=(number_prefix >= tmp.lexeme.length())
                | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
                | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
                | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
                | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<4
                | (tmp.lexeme.substr(number_prefix,2)=="LU")<<5
                | (tmp.lexeme.substr(number_prefix,2)=="uL")<<6
                | (tmp.lexeme.substr(number_prefix,2)=="UL")<<7;
            if (!digit_encountered || !valid_integer_suffix) {
                tmp.lexeme = "";
                tmp.token_type = ERROR;
                if (valid_integer_suffix >> 1) ++number_prefix;
                if (valid_integer_suffix >> 4) ++number_prefix;
                if (number_prefix < tmp.lexeme.length())
                    input.UngetString(tmp.lexeme.substr(number_prefix));
                /*
                //Không có digit --> 0x____________[xxx]
                                    0x____________uL (nuốt luôn chữ uL)
                                    0x____________uLA (nhả chữ A)
                                    0x12312_[(Có chữ)] và chữ đó khác */
            }
            else {
                tmp.token_type = HEXINT;
                if (valid_integer_suffix >> 1) ++number_prefix;
                if (valid_integer_suffix >> 4) ++number_prefix;
                //Nhả sau cái integer suffix về...
                if (number_prefix < tmp.lexeme.length())
                    input.UngetString(tmp.lexeme.substr(number_prefix));
            }
            return tmp;
        }
        else if (toupper(tmp.lexeme[1])=='B') {
            ++number_prefix;
            int digit_encountered=0;
            for (char c: tmp.lexeme.substr(2)) {
                if (c=='0' || c=='1') {
                    digit_encountered=1;
                    ++number_prefix;
                }
                else if (c=='_') {
                    ++number_prefix;
                }
                else break;
            }
            int valid_integer_suffix=(number_prefix >= tmp.lexeme.length())
                | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
                | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
                | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
                | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<4
                | (tmp.lexeme.substr(number_prefix,2)=="LU")<<5
                | (tmp.lexeme.substr(number_prefix,2)=="uL")<<6
                | (tmp.lexeme.substr(number_prefix,2)=="UL")<<7;
            if (!digit_encountered || !valid_integer_suffix) {
                tmp.lexeme = "";
                tmp.token_type = ERROR;
                if (valid_integer_suffix >> 1) ++number_prefix;
                if (valid_integer_suffix >> 4) ++number_prefix;
                if (number_prefix < tmp.lexeme.length())
                    input.UngetString(tmp.lexeme.substr(number_prefix));
            }
            else {
                tmp.token_type = BININT;
                if (valid_integer_suffix >> 1) ++number_prefix;
                if (valid_integer_suffix >> 4) ++number_prefix;
                if (number_prefix < tmp.lexeme.length())
                    input.UngetString(tmp.lexeme.substr(number_prefix));
            }
            return tmp;
        }
        else {
            int value=0;
            for (char ch : tmp.lexeme)
                if (isdigit(ch)) {
                    value=value*10+(ch-'0');
                    ++number_prefix;
                }
                else if (ch=='_') {
                    ++number_prefix;
                }
                else break;
            //else return OctalInteger();
            //According to old grammar, 030 is octal literal.
            //But octal literals like 010 are no longer supported in latest version
            //The dmd compiler raises error for octal literal larger than 7.
            //Therefore, we raise an error for, example, 000000009 or 0010
            int valid_integer_suffix=(number_prefix >= tmp.lexeme.length())
                | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
                | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
                | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
                | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<4
                | (tmp.lexeme.substr(number_prefix,2)=="LU")<<5
                | (tmp.lexeme.substr(number_prefix,2)=="uL")<<6
                | (tmp.lexeme.substr(number_prefix,2)=="UL")<<7;
            if (value>7 || !valid_integer_suffix) {
                tmp.lexeme="";
                tmp.token_type=ERROR;
                if (valid_integer_suffix >> 1) ++number_prefix;
                if (valid_integer_suffix >> 4) ++number_prefix;
                if (number_prefix < tmp.lexeme.length())
                    input.UngetString(tmp.lexeme.substr(number_prefix));
                return tmp;
            }
            tmp.token_type=OCTINT;
            if (valid_integer_suffix >> 1) ++number_prefix;
            if (valid_integer_suffix >> 4) ++number_prefix;
            if (number_prefix < tmp.lexeme.length())
                input.UngetString(tmp.lexeme.substr(number_prefix));
            return tmp;
        }
    }
    else { //Not start with '0' --> DECINT.
        int number_prefix=0;
        int digit_encountered=0;
        for (char c: tmp.lexeme) {
            if (isdigit(c)) {
                digit_encountered=1;
                ++number_prefix;
            }
            else if (c=='_') {
                ++number_prefix;
            }
            else break;
        }
        int valid_integer_suffix=(number_prefix >= tmp.lexeme.length())
            | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
            | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
            | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
            | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<4
            | (tmp.lexeme.substr(number_prefix,2)=="LU")<<5
            | (tmp.lexeme.substr(number_prefix,2)=="uL")<<6
            | (tmp.lexeme.substr(number_prefix,2)=="UL")<<7;
        if (!digit_encountered || !valid_integer_suffix) {
            tmp.lexeme = "";
            tmp.token_type = ERROR;
            if (valid_integer_suffix >> 1) ++number_prefix;
            if (valid_integer_suffix >> 4) ++number_prefix;
            if (number_prefix < tmp.lexeme.length())
                input.UngetString(tmp.lexeme.substr(number_prefix));
        }
        else {
            tmp.token_type = DECINT;
            if (valid_integer_suffix >> 1) ++number_prefix;
            if (valid_integer_suffix >> 4) ++number_prefix;
            if (number_prefix < tmp.lexeme.length())
                input.UngetString(tmp.lexeme.substr(number_prefix));
        }
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

Token LexicalAnalyzer::ErrorToken()
{
    Token tmp;
    tmp.line_no = line_no;
    tmp.token_type = ERROR;
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (c=='\'')
            {
                input.UngetChar(c);
                return WysString();
            }
            else if (c=='\"')
            {
                input.UngetChar(c);
                return DoubleQuoteString();
            }
            else if (isalpha(c) && (c=='r' || c=='q')) 
            {
                input.UngetChar(c);
                return ScanString();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } 
            else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    //lexer.input.stream = &std::cin;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}

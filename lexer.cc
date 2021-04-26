#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR",
    "DECINT", "BININT", "HEXINT", "OCTINT"
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
    if (isdigit(c)) { //surevkl '0'
        tmp.lexeme = "";
        tmp.lexeme+=c;
        input.GetChar(c);
        if (toupper(c)=='X') {
            tmp.lexeme+=c;
            return HexadecimalInteger();
        }
        else if (toupper(c)=='B') {
            tmp.lexeme+=c;
            return BinaryInteger();
        }

        //else return OctalInteger();
        //According to old grammar, 030 is octal literal.
        //But octal literals like 010 are no longer supported in latest version
        //The dmd compiler raises error for octal literal larger than 7.
        //Therefore, we raise an error for, example, 000000009 or 0010
        int value=0;
        while (!input.EndOfInput() && (isdigit(c) || c=='_')) {
            tmp.lexeme += c;
            if (isdigit(c)) value=value*10+(c-'0');
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (value<=7) {
            tmp.token_type = OCTINT;
            tmp.line_no = line_no;
        }
        else {
            tmp.lexeme = "";
            tmp.token_type = ERROR;
            tmp.line_no = line_no;
        }
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::DecimalInteger() {
    char c;
    input.GetChar(c);
    int digit_encountered=0;
    while (!input.EndOfInput() && (isdigit(c)||c=='_')) {
        tmp.lexeme += c;
        digit_encountered |= isdigit(c);
        input.GetChar(c);
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    if (digit_encountered) {
        tmp.token_type = DECINT;
        tmp.line_no = line_no;
    }
    else {
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
    }
    return tmp;
}

Token LexicalAnalyzer::BinaryInteger() {
    char c;
    input.GetChar(c);
    int digit_encountered=0;
    while (!input.EndOfInput() && (c=='0'||c=='1'||c=='_')) {
        tmp.lexeme += c;
        digit_encountered |= (c=='0'||c=='1');
        input.GetChar(c);
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    if (digit_encountered) {
        tmp.token_type = BININT;
        tmp.line_no = line_no;
    }
    else {
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
    }
    return tmp;
}

Token LexicalAnalyzer::HexadecimalInteger() {
    char c;
    input.GetChar(c);
    int digit_encountered=0;
    while (!input.EndOfInput() && (isxdigit(c)||c=='_')) {
        tmp.lexeme += c;
        digit_encountered |= isxdigit(c);
        input.GetChar(c);
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    if (digit_encountered) {
        tmp.token_type = HEXINT;
        tmp.line_no = line_no;
    }
    else {
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
    }
    return tmp;
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
            if (isdigit(c) && c=='0') {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isdigit(c)) {
                input.UngetChar(c);
                return DecimalInteger();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}

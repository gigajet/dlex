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
    "PLUS", "MINUS", "DIV", "MULT", "REMAIN", "POWER",
	"INC", "DEC",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
	"ASSIGN", "PLUSASSIGN", "MINUSASSIGN", "DIVASSIGN", "REMAINASSIGN", "MULTASSIGN", "POWERASSIGN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
	"NOT", "LogicOR", "LogicAND",
	"BitOR", "BitAND", "XOR", "ONECOMPLETE", "LEFTSHIFT", "RIGHTSHIFT", "LogicRIGHTSHIFT",
	"ORASSIGN", "ANDASSIGN", "XORASSIGN", "ONECOMPLETE_ASSIGN", "LEFTSHIFT_ASSIGN", "RIGHTSHIFT_ASSIGN",
	"LOG_RIGHTSHIFT_ASSIGN",
	"LAMBDA",
	"CONDITIONAL", "QMARK",
    "DOT", "NUM", "ID", "ERROR",
	"CHAR",
    "DECINT", "BININT", "HEXINT", "OCTINT"
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };
#define ESC_SEQUENCE_COUNT 12
char esc_sequence[] = { '\'', '\"', '\?', '\\', '0' ,'a', 'b', 'f', 'n', 'r', 't', 'v' };

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
bool LexicalAnalyzer::IsEscSequence(char a) {
	for (int i = 0; i < ESC_SEQUENCE_COUNT; i++) {
		if (a == esc_sequence[i]) {
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

Token LexicalAnalyzer::ScanChar() {
	char c;
	string tempo = "";
	input.GetChar(c);
	if (c == '\'') {
		tmp.lexeme += c;
		input.GetChar(c);
		tempo += c;
		if (c != '\\') { //Not an escape sequence
			input.GetChar(c);
			tempo += c;
			if (c == '\'') {
				tmp.lexeme += tempo;
				tmp.line_no = line_no;
				tmp.token_type = CHAR;
			}
			else {
				if (!input.EndOfInput()) {
					input.UngetString(tempo);
				}
				tmp.lexeme = "";
				tmp.token_type = ERROR;
			}
		}
		else {
			input.GetChar(c);
			tempo += c;
			if (IsEscSequence(c)) {
				input.GetChar(c);
				tempo += c;
				if (c == '\'') {
					tmp.lexeme += tempo;
					tmp.line_no = line_no;
					tmp.token_type = CHAR;
				}
				else {
					if (!input.EndOfInput()) {
						input.UngetString(tempo);
					}
					tmp.lexeme = "";
					tmp.token_type = ERROR;
				}
			}
			else {
				if (!input.EndOfInput()) {
					input.UngetString(tempo);
				}
				tmp.lexeme = "";
				tmp.token_type = ERROR;
			}
		}
	}
	return tmp;
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
			input.GetChar(c);
			if (c == '+') tmp.token_type = INC;
			else if (c == '=') tmp.token_type = PLUSASSIGN;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = PLUS;
			}
			return tmp;
		case '-':
			input.GetChar(c);
			if (c == '-') tmp.token_type = DEC;
			else if (c == '=') tmp.token_type = MINUSASSIGN;
			else
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = MINUS;
			}
			return tmp;
		case '/':
			input.GetChar(c);
			if (c == '=') tmp.token_type = DIVASSIGN;
			else
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = DIV;
			}
			return tmp;
		case '%':
			input.GetChar(c);
			if (c == '=') tmp.token_type = REMAINASSIGN;
			else
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = REMAIN;
			}
			return tmp;
		case '*':
			input.GetChar(c);
			if (c == '=') tmp.token_type = MULTASSIGN;
			else
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = MULT;
			}
			return tmp;
		case '=':
			input.GetChar(c);
			if (c == '=') tmp.token_type = EQUAL;
			else if (c == '>') tmp.token_type = LAMBDA;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = ASSIGN;
			}
			return tmp;
		case '!':
			input.GetChar(c);
			if (c == '=') tmp.token_type = NOTEQUAL;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = NOT;
			}
			return tmp;
		case '&':
			input.GetChar(c);
			if (c == '&') tmp.token_type = LAND;
			else if (c == '=') tmp.token_type = ANDASSIGN;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = BAND;
			}
			return tmp;
		case '|':
			input.GetChar(c);
			if (c == '|') tmp.token_type = LOR;
			else if (c == '=') tmp.token_type = ORASSIGN;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = BOR;
			}
			return tmp;
		case '^':
			input.GetChar(c);
			if (c == '=') tmp.token_type = XORASSIGN;
			else if (c == '^') {
				input.GetChar(c);
				if (c == '=') tmp.token_type = POWERASSIGN;
				else {
					if (!input.EndOfInput()) {
						input.UngetChar(c);
					}
					tmp.token_type = POWER;
				}
			}
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = XOR;
			}
			return tmp;
		case '~':
			input.GetChar(c);
			if (c == '=') tmp.token_type = ONECOMPLETE_ASSIGN;
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}	
				tmp.token_type = ONECOMPLETE;
			}
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
			}
			else if (c == '>') {
				tmp.token_type = NOTEQUAL;
			}
			else if (c == '<') {
				input.GetChar(c);
				if (c == '=') tmp.token_type = LEFTSHIFT_ASSIGN;
				else {
					if (!input.EndOfInput()) {
						input.UngetChar(c);
					}
					tmp.token_type = LEFTSHIFT;
				}
			}
			else {
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
			}
			else if (c == '>') {
				input.GetChar(c);
				if (c == '>') {
					input.GetChar(c);
					if (c == '=') { tmp.token_type = LOG_RIGHTSHIFT_ASSIGN; }
					else {
						if (!input.EndOfInput()) {
							input.UngetChar(c);
						}
						tmp.token_type = LOGRIGHTSHIFT;
					}
				}
				else if (c == '=') tmp.token_type = RIGHTSHIFT_ASSIGN;
				else {
					if (!input.EndOfInput()) {
						input.UngetChar(c);
					}
					tmp.token_type = RIGHTSHIFT;
				}
			}
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = GREATER;
			}
			return tmp;
		case '?':
			input.GetChar(c);
			if (c == ':') {
				tmp.token_type = CONDITIONAL;
			}
			else {
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = QMARK;
			}
			return tmp;
		case '\'':
			input.UngetChar(c);
			return ScanChar();
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

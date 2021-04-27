#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>

#include "lexer.h"
#include "inputbuf.h"
#include "Util.h"

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
    "DECINT", "BININT", "HEXINT", "OCTINT",
    "WYSIWYGSTR", "DOUBLESTR", "DELIMITEDSTR", "TOKENSTR",
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };
#define ESC_SEQUENCE_COUNT 12
char esc_sequence[] = { '\'', '\"', '\?', '\\', '0' ,'a', 'b', 'f', 'n', 'r', 't', 'v' };

vector<string> namecharEntity;
vector<string> getEntity(string filename) {
	vector<string> ret;
	fstream newfile;
	newfile.open(filename, ios::in); 
	if (newfile.is_open()) {  
		//cout << "File exist" << endl;
		string tp;
		while (getline(newfile, tp)) { 
			ret.push_back(tp);
		}
		newfile.close(); //close the file object.
	}
	return ret;
}


bool isHexa(int c) {
	if (isdigit(c) || int(c) < 71 & int(c) > 64 || int(c) < 103 & int(c) > 96) {
		return true;
	}
	else return false;
}

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

bool LexicalAnalyzer::IsEscSequence(char a) {
	for (int i = 0; i < ESC_SEQUENCE_COUNT; i++) {
		if (a == esc_sequence[i]) {
			return true;
		}
	}
	return false;
}

bool LexicalAnalyzer::IsValidEntity(string s) {
	for (int i = 0; i < namecharEntity.size(); i++) {
		if (s == namecharEntity[i]) return true;
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

Token LexicalAnalyzer::ScanChar() {
	char c;
	string tempo = "";
	input.GetChar(c);
	if (c == '\'') {
		tmp.lexeme += c;
		input.GetChar(c);
		tempo += c;
		if (c == '\n') {
			tmp.lexeme = "";
			tmp.token_type = ERROR;
			return tmp;
		}
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
		else {//Is an escape sequence
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
			else if (c == 'x') {
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while(!input.EndOfInput() && i < 4 && isHexa(c)){	
					i++;
					input.GetChar(c);
					tempo += c;			
				}
				if (i == 2) {
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
			else if (c == 'u') {
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while (!input.EndOfInput() && i < 4 && isHexa(c)) {
					i++;
					input.GetChar(c);
					tempo += c;
				}
				if (i == 4) {
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
			else if (c == 'U') {
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while (!input.EndOfInput() && i < 8 && isHexa(c)) {
					i++;
					input.GetChar(c);
					tempo += c;
				}
				if (i == 8) {
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
			else if (c == '&') {
				int i = 0;
				string t = "";
				while (!input.EndOfInput() && i <= 8) {
					i++;
					input.GetChar(c);
					tempo += c;
					if (c == ';') break;
					t += c;
				}
				if (c == ';') {
					if (IsValidEntity(t)) {
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
	namecharEntity = getEntity("C:\\Users\\Admin\\Downloads\\Principal of Programming Language\\Dlang\\dlex-main\\dlex-main\\CharEntity.txt");
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

#include "lexer.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    //keywords
    "ABSTRACT", "ALIAS", "ALIGN", "ASM", "ASSERT", "AUTO", "BODY", "BOOL", "BREAK", "BYTE",
    "CASE", "CAST", "CATCH", "CDOUBLE", "CENT", "CFLOAT", "CHAR", "CLASS", "CONST",
    "CONTINUE", "CREAL", "DCHAR", "DEBUG", "DEFAULT", "DELEGATE", "DELETE",
    "DEPRECATED", "DO", "DOUBLE", "ELSE", "ENUM", "EXPORT", "EXTERN", "FALSE",
    "FINAL", "FINALLY", "FLOAT", "FOR", "FOREACH", "FOREACH_REVERSE", "FUNCTION",
    "GOTO", "IDOUBLE", "IF", "IFLOAT", "IMMUTABLE", "IMPORT", "IN", "INOUT", "INT",
    "INTERFACE", "INVARIANT", "IREAL", "IS", "LAZY", "LONG", "MACRO",
    "MIXIN", "MODULE", "NEW", "NOTHROW", "NULL", "OUT", "OVERRIDE", "PACKAGE", "PRAGMA",
    "PRIVATE", "PROTECTED", "PUBLIC", "PURE", "REAL", "REF", "RETURN", "SCOPE", "SHARED",
    "SHORT", "STATIC", "STRUCT", "SUPER", "SWITCH", "SYNCHRONIZED", "TEMPLATE", "THIS",
    "THROW", "TRUE", "TRY", "TYPEID", "TYPEOF", "UBYTE", "UCENT", "UINT", "ULONG", "UNION",
    "UNITTEST", "USHORT", "VERSION", "VOID", "WCHAR", "WHILE", "WITH", "__FILE__",
    "__FILE_FULL_PATH__", "__MODULE__", "__LINE__", "__FUNCTION__",
    "__PRETTY_FUNCTION__", "__GSHARED", "__TRAITS", "__VECTOR", "__PARAMETERS",
	"LCURLY", "RCURLY",
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
    "DOT", "SLICE", "DOTDOTDOT", "NUM", "ID", "ERROR",
	"CHARACTER", "UNICHARACTER",
    "DECINT", "BININT", "HEXINT", "OCTINT",
    "WYSIWYGSTR", "DOUBLESTR", "DELIMITEDSTR", "TOKENSTR",
	"FLOATNUM", "DOUBLENUM", "REALNUM", "COMPLEXFLOAT", "COMPLEXDOUBLE", "COMPLEXREAL",
  "INLINE CMT", "BLOCKCMT", "NESTED CMT"
};

#define KEYWORDS_COUNT 110
string keyword[] = { 
    "abstract", "alias", "align", "asm", "assert", "auto", "body", "bool", "break", "byte",
    "case", "cast", "catch", "cdouble", "cent", "cfloat", "char", "class", "const",
    "continue", "creal", "dchar", "debug", "default", "delegate", "delete",
    "deprecated", "do", "double", "else", "enum", "export", "extern", "false",
    "final", "finally", "float", "for", "foreach", "foreach_reverse", "function",
    "goto", "idouble", "if", "ifloat", "immutable", "import", "in", "inout", "int",
    "interface", "invariant", "ireal", "is", "lazy", "long", "macro",
    "mixin", "module", "new", "nothrow", "null", "out", "override", "package", "pragma",
    "private", "protected", "public", "pure", "real", "ref", "return", "scope", "shared",
    "short", "static", "struct", "super", "switch", "synchronized", "template", "this",
    "throw", "true", "try", "typeid", "typeof", "ubyte", "ucent", "uint", "ulong", "union",
    "unittest", "ushort", "version", "void", "wchar", "while", "with", "__FILE__",
    "__FILE_FULL_PATH__", "__MODULE__", "__LINE__", "__FUNCTION__",
    "__PRETTY_FUNCTION__", "__gshared", "__traits", "__vector", "__parameters", 
	};

#define ESC_SEQUENCE_COUNT 12
char esc_sequence[] = { '\'', '\"', '\?', '\\', '0' ,'a', 'b', 'f', 'n', 'r', 't', 'v' };

void debugg(string messages)
{
  #ifdef DEBUGG
    cout<<messages<<endl;
  #endif
}

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
bool isOctal(int c) {
	if (isdigit(c) && int(c) < 56) 
	{
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
		#ifdef __APPLE__
			if (s == namecharEntity[i].substr(0, namecharEntity[i].length()-1)) return true;
		#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
			if (s == namecharEntity[i]) return true;
		#elif defined(__unix__) 
			if (s == namecharEntity[i].substr(0, namecharEntity[i].length()-1)) return true;
		#endif
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

/*
	Vì không xử lý hết 1 đống issue [trên discord] nên dẹp code lại luôn.
*/
// Token LexicalAnalyzer::ScanNumber_oldvkl()
// {
//     char c;
//     input.GetChar(c);
//     tmp.line_no = line_no;
// 	//isDelimeterOfToken chứa dấu chấm
// 	//chỉ đọc tối đa 1 dấu chấm
// 	int dot_count=0;
//     while (!input.EndOfInput() && ((dot_count==0 && c=='.') || (!isspace(c) && !isDelimiterOfToken(c)))) {
//         tmp.lexeme += c;
// 		dot_count += (c=='.');
//         input.GetChar(c);
//     }
//     if (!input.EndOfInput()) {
//         input.UngetChar(c);
//     }
//     if (tmp.lexeme.length()==1 && isdigit(tmp.lexeme[0])) {
//         tmp.token_type = DECINT;
//         return tmp;
//     }
//     else if (tmp.lexeme[0]=='0') {
//         int number_prefix=1;
//         if (toupper(tmp.lexeme[1])=='X') {
// 			//Base 16 HEX
//             ++number_prefix;
//             int digit_encountered=0;
// 			int dot_encountered=0;
//             for (char c: tmp.lexeme.substr(2)) {
// 				dot_encountered += (c=='.');
//                 if (isxdigit(c)) {
//                     digit_encountered=1;
//                     ++number_prefix;
//                 }
//                 else if (c=='_') {
//                     ++number_prefix;
//                 }
//                 else break;
//             }
// 			/*
// 				Suffix Li | [Ff][i | eps] là vô nghĩa với hex vì:
// 				Dlang không cho suffix i/Li (vd 0x33i hay 0x33Li là không được)
// 				suffix f và F được hiểu là chữ số hexa nên không bao giờ được có suffix F/f
// 			*/
//             int valid_integer_suffix=(number_prefix >= tmp.lexeme.length() ? 1 : 0)
//                 | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
//                 | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
//                 | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
// 				| (tmp.lexeme.substr(number_prefix,1)=="p")<<4 //Hex exponent
// 				| (tmp.lexeme.substr(number_prefix,1)=="P")<<5
//                 | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<7
//                 | (tmp.lexeme.substr(number_prefix,2)=="LU")<<8
//                 | (tmp.lexeme.substr(number_prefix,2)=="uL")<<9
//                 | (tmp.lexeme.substr(number_prefix,2)=="UL")<<10;
//             if (!digit_encountered || !valid_integer_suffix || dot_encountered) {
//                 tmp.token_type = ERROR;
//                 if (valid_integer_suffix >> 1) ++number_prefix;
//                 if (valid_integer_suffix >> 7) ++number_prefix;
//                 if (number_prefix < tmp.lexeme.length())
//                     input.UngetString(tmp.lexeme.substr(number_prefix));
//                 tmp.lexeme = "";
//                 /*
//                 //Không có digit --> 0x____________[xxx]
//                                     0x____________uL (nuốt luôn chữ uL)
//                                     0x____________uLA (nhả chữ A)
//                                     0x12312_[(Có chữ)] và chữ đó khác */
//             }
//             else {
// 				//Nếu bit 4,5 bật (Exponent P) thì bay vào inreal
// 				if (valid_integer_suffix & 0x30) {
// 					input.UngetString(tmp.lexeme);
// 					tmp.lexeme="";
// 					return inreal();
// 				}
//                 tmp.token_type = HEXINT;
//                 if (valid_integer_suffix >> 1) ++number_prefix;
//                 if (valid_integer_suffix >> 7) ++number_prefix;
//                 //Nhả sau cái integer suffix về...
//                 if (number_prefix < tmp.lexeme.length()) {
//                     input.UngetString(tmp.lexeme.substr(number_prefix));
// 					tmp.lexeme = tmp.lexeme.substr(0,number_prefix);
// 				}
//             }
//             return tmp;
//         }
//         else if (toupper(tmp.lexeme[1])=='B') {
// 			//Base 2, Binary
//             ++number_prefix;
//             int digit_encountered=0;
//             for (char c: tmp.lexeme.substr(2)) {
//                 if (c=='0' || c=='1') {
//                     digit_encountered=1;
//                     ++number_prefix;
//                 }
//                 else if (c=='_') {
//                     ++number_prefix;
//                 }
//                 else break;
//             }
// 			//There is no binary float --> No floating point suffix supported for 0b...(Li | [fF][i|eps])
//             int valid_integer_suffix=(number_prefix >= tmp.lexeme.length())
//                 | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
//                 | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
//                 | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
//                 | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<4
//                 | (tmp.lexeme.substr(number_prefix,2)=="LU")<<5
//                 | (tmp.lexeme.substr(number_prefix,2)=="uL")<<6
//                 | (tmp.lexeme.substr(number_prefix,2)=="UL")<<7;
//             if (!digit_encountered || !valid_integer_suffix) {
//                 tmp.token_type = ERROR;
//                 if (valid_integer_suffix >> 1) ++number_prefix;
//                 if (valid_integer_suffix >> 4) ++number_prefix;
//                 if (number_prefix < tmp.lexeme.length())
//                     input.UngetString(tmp.lexeme.substr(number_prefix));
//                 tmp.lexeme = "";
//             }
//             else {
//                 tmp.token_type = BININT;
//                 if (valid_integer_suffix >> 1) ++number_prefix;
//                 if (valid_integer_suffix >> 4) ++number_prefix;
//                 if (number_prefix < tmp.lexeme.length()) {
//                     input.UngetString(tmp.lexeme.substr(number_prefix));
// 					tmp.lexeme = tmp.lexeme.substr(0,number_prefix);
// 				}
//             }
//             return tmp;
//         }
//         else {
//             int value=0;
//             for (char ch : tmp.lexeme)
//                 if (isdigit(ch)) {
//                     value=value*10+(ch-'0');
//                     ++number_prefix;
//                 }
//                 else if (ch=='_') {
//                     ++number_prefix;
//                 }
//                 else break;
//             //else return OctalInteger();
//             //According to old grammar, 030 is octal literal.
//             //But octal literals like 010 are no longer supported in latest version
//             //The dmd compiler raises error for octal literal larger than 7.
//             //Therefore, we raise an error for, example, 000000009 or 0010
//             int valid_integer_suffix=(number_prefix >= tmp.lexeme.length() ? 1 : 0)
//                 | (tmp.lexeme.substr(number_prefix,1)=="L")<<1
//                 | (tmp.lexeme.substr(number_prefix,1)=="u")<<2
//                 | (tmp.lexeme.substr(number_prefix,1)=="U")<<3
//                 | (tmp.lexeme.substr(number_prefix,1)=="f")<<4
//                 | (tmp.lexeme.substr(number_prefix,1)=="F")<<5
//                 | (tmp.lexeme.substr(number_prefix,1)=="i")<<6
//                 | (tmp.lexeme.substr(number_prefix,1)=="e")<<7
//                 | (tmp.lexeme.substr(number_prefix,1)=="E")<<8
//                 | (tmp.lexeme.substr(number_prefix,2)=="Lu")<<9
//                 | (tmp.lexeme.substr(number_prefix,2)=="LU")<<10
//                 | (tmp.lexeme.substr(number_prefix,2)=="uL")<<11
//                 | (tmp.lexeme.substr(number_prefix,2)=="UL")<<12
//                 | (tmp.lexeme.substr(number_prefix,2)=="Fi")<<13
//                 | (tmp.lexeme.substr(number_prefix,2)=="fi")<<14
//                 | (tmp.lexeme.substr(number_prefix,2)=="Li")<<15;
//             if (value>7 || !valid_integer_suffix) {
//                 tmp.token_type=ERROR;
//                 if (valid_integer_suffix >> 1) ++number_prefix;
//                 if (valid_integer_suffix >> 9) ++number_prefix;
//                 if (number_prefix < tmp.lexeme.length())
//                     input.UngetString(tmp.lexeme.substr(number_prefix));
//                 tmp.lexeme="";
//                 return tmp;
//             }
// 			//Nếu các bit 4,5,6,7,8,9,13,14,15 bật thì nhả string rồi bay qua inreal
// 			//C++14 trở lên mới hỗ trợ 0b
// 			//Đổi: 0b1110_0011_1111_0000 = 0xE3F0
// 			//TODO xem lại OCT với floating suffix
// 			if (valid_integer_suffix & 0xE3F0) {
// 				input.UngetString(tmp.lexeme);
// 				tmp.lexeme="";
// 				return inreal();
// 			}
//             tmp.token_type=OCTINT;
//             if (valid_integer_suffix >> 1) ++number_prefix;
//             if (valid_integer_suffix >> 9) ++number_prefix;
// 			if (number_prefix < tmp.lexeme.length()) {
// 				input.UngetString(tmp.lexeme.substr(number_prefix));
// 				tmp.lexeme = tmp.lexeme.substr(0,number_prefix);
// 			}
//             return tmp;
//         }
//     }
//     else { //Not start with '0' --> DECINT.
//         int number_prefix=0;
//         int digit_encountered=0;
//         for (char c: tmp.lexeme) {
//             if (isdigit(c)) {
//                 digit_encountered=1;
//                 ++number_prefix;
//             }
//             else if (c=='_') {
//                 ++number_prefix;
//             }
//             else break;
//         }
// 		int valid_integer_suffix=(number_prefix >= tmp.lexeme.length() ? 1 : 0)
// 			| (tmp.lexeme.substr(number_prefix,1)=="L")<<1
// 			| (tmp.lexeme.substr(number_prefix,1)=="u")<<2
// 			| (tmp.lexeme.substr(number_prefix,1)=="U")<<3
// 			| (tmp.lexeme.substr(number_prefix,1)=="f")<<4
// 			| (tmp.lexeme.substr(number_prefix,1)=="F")<<5
// 			| (tmp.lexeme.substr(number_prefix,1)=="i")<<6
// 			| (tmp.lexeme.substr(number_prefix,1)=="e")<<7
// 			| (tmp.lexeme.substr(number_prefix,1)=="E")<<8
// 			| (tmp.lexeme.substr(number_prefix,2)=="Lu")<<9
// 			| (tmp.lexeme.substr(number_prefix,2)=="LU")<<10
// 			| (tmp.lexeme.substr(number_prefix,2)=="uL")<<11
// 			| (tmp.lexeme.substr(number_prefix,2)=="UL")<<12
// 			| (tmp.lexeme.substr(number_prefix,2)=="Fi")<<13
// 			| (tmp.lexeme.substr(number_prefix,2)=="fi")<<14
// 			| (tmp.lexeme.substr(number_prefix,2)=="Li")<<15;
//         if (!digit_encountered || !valid_integer_suffix) {
//             tmp.lexeme = "";
//             tmp.token_type = ERROR;
// 			if (valid_integer_suffix >> 1) ++number_prefix;
// 			if (valid_integer_suffix >> 9) ++number_prefix;
//             if (number_prefix < tmp.lexeme.length())
//                 input.UngetString(tmp.lexeme.substr(number_prefix));
//         }
//         else {
// 			//Nếu các bit 4,5,6,7,8,9,13,14,15 bật thì nhả string rồi bay qua inreal
// 			//C++14 trở lên mới hỗ trợ 0b
// 			//Đổi: 0b1110_0011_1111_0000 = 0xE3F0
// 			//TODO xem lại OCT với floating suffix
// 			if (valid_integer_suffix & 0xE3F0) {
// 				input.UngetString(tmp.lexeme);
// 				tmp.lexeme="";
// 				return inreal();
// 			}
//             tmp.token_type = DECINT;
// 			if (valid_integer_suffix >> 1) ++number_prefix;
// 			if (valid_integer_suffix >> 9) ++number_prefix;
// 			if (number_prefix < tmp.lexeme.length()) {
// 				input.UngetString(tmp.lexeme.substr(number_prefix));
// 				tmp.lexeme = tmp.lexeme.substr(0,number_prefix);
// 			}
//         }
//         return tmp;
//     }
// }


Token LexicalAnalyzer::ScanNumber() {
    char c,c1;
    input.GetChar(c);
	string lexeme; lexeme += c;
    tmp.line_no = line_no;
	int base=10;
	bool error=false;
	int digit=0;
	bool anyDigit=false, anyHexDigit=false, anyBinDigit=false;
	long long n=0;
	if (c=='0') {
		input.GetChar(c); lexeme+=c;
		switch (c) {
			case '0': case '1': case '2': case '3': case '4': 
			case '5': case '6': case '7': case '8': case '9':
				base=8;
				break;
			case 'x': case 'X':
				base=16;
				break;
			case 'b': case 'B':
				base=2;
				break;
			case '.':
				input.GetChar(c1); lexeme+=c1;
                if (c1 == '.')
                    goto Ldone; // if ".."
                if (isalpha(c1) || c1 == '_' || c1 & 0x80)
                    goto Ldone; // if ".identifier" or ".unicode"
                goto Lreal; // '.' is part of current token
            case 'i':
            case 'f':
            case 'F':
                goto Lreal;
            case '_':
                base = 8;
                break;
            case 'L':
				input.GetChar(c1); lexeme+=c1;
                if (c1 == 'i')
                    goto Lreal;
                break;
            default:
                break;
		}
	}
	while (true) {
		input.GetChar(c); lexeme+=c;
		switch (c) {
			case '0': case '1': 
				digit = c - '0';
				anyBinDigit=true;
				anyDigit=true;
				anyHexDigit=true;
				break;
			case '2': case '3': case '4': case '5': 
			case '6': case '7': case '8': case '9':
				digit = c - '0';
				anyDigit=true;
				anyHexDigit=true;
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				if (base != 16)
				{
					if (c == 'e' || c == 'E' || c == 'f' || c == 'F')
						goto Lreal;
				}
				anyHexDigit=true;
				if (c >= 'a')
					digit = c + 10 - 'a';
				else
					digit = c + 10 - 'A';
				break;
			case 'L':
				input.GetChar(c1); lexeme+=c1;
				if (c1 == 'i')
					goto Lreal;
				goto Ldone;
			case '.':
				input.GetChar(c1); lexeme+=c1;
				if (c1 == '.')
					goto Ldone; // ..
				if (base == 10 && (isalpha(c1) || c1 == '_' || c1 & 0x80))
					goto Ldone; // .iden .unicode
				if (base == 16 && (!isxdigit(c1) || c1 == '_' || c1 & 0x80))
					goto Ldone; // .iden .unicode
				if (base == 2)
					goto Ldone; // no . in binary
				goto Lreal;
			case 'p': case 'P': case 'i':
			Lreal:
				input.UngetString(lexeme);
				return inreal();
			case '_':
				continue;
			default:
				goto Ldone1;
		}
		if (digit >= base) {
			error = true;
		}
		//Just check if value >=8 or not
		n=n*base+digit; if (n>=8) n=8;
	}
	goto Ldone2;
	Ldone:
		input.UngetChar(c1); lexeme.pop_back(); //These steps are necessary
	Ldone1:
		input.UngetChar(c); lexeme.pop_back();
	Ldone2:
		if ((base==2&&!anyBinDigit) || (base==16&&!anyHexDigit)) {
			error=true;
		}
		
		int flag_suffix=0;
		//bit 0: u, bit 1: L
        // Parse trailing 'u', 'U', 'l' or 'L' in any combination
		bool still_reading=true;
        while (still_reading) {
            int f; input.GetChar(c); lexeme+=c;
            switch (c) {
				case 'U': case 'u':
					if (flag_suffix & 1) {
						//Already has the flag
						error=true; 
					}
					flag_suffix |= 1;
					break;
				case 'L':
					if (flag_suffix & 2) {
						//Already has the flag
						error=true; 
					}
					flag_suffix |= 2;
					break;
				default:
					input.UngetChar(c); lexeme.pop_back();
					still_reading=false;
					break;
            }
        }
		if (base==8 && n>=8) error=true;
	
	if (error) {
		tmp.token_type=ERROR;
	}
	else {
		tmp.lexeme = lexeme;
		switch (base) {
			case 2: tmp.token_type=BININT; break;
			case 8: tmp.token_type=OCTINT; break;
			case 10: tmp.token_type=DECINT; break;
			case 16: tmp.token_type=HEXINT; break;
			default:
				tmp.token_type=ERROR;
				break;
		}
	}
	return tmp;
}

Token LexicalAnalyzer::ScanChar() {
	char c;
	string tempo = "";
	input.GetChar(c);
	if (c == '\'') 
	{
		tmp.lexeme += c;
		input.GetChar(c);
		tempo += c;
		if (c == '\n') 
		{
			tmp.lexeme = "";
			tmp.token_type = ERROR;
			return tmp;
		}
		if (c != '\\') //Not an escape sequence
		{ 
			input.GetChar(c);
			tempo += c;
			if (c == '\'') 
			{
				tmp.lexeme += tempo;
				tmp.line_no = line_no;
				tmp.token_type = CHARACTER;
			}
			else 
			{
				if (!input.EndOfInput()) {
					input.UngetString(tempo);
				}
				tmp.lexeme = "";
				tmp.token_type = ERROR;
			}
		}
		else //Is an escape sequence
		{
			input.GetChar(c);
			tempo += c;
			if (IsEscSequence(c)) 
			{
				input.GetChar(c);
				tempo += c;
				if (c == '\'') {
					tmp.lexeme += tempo;
					tmp.line_no = line_no;
					tmp.token_type = UNICHARACTER;
				}
				else {
					if (!input.EndOfInput()) {
						input.UngetString(tempo);
					}
					tmp.lexeme = "";
					tmp.token_type = ERROR;
				}
			}
			else if (isOctal(c)) 
			{
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while (!input.EndOfInput() && i < 4)
				{
					i++;
					input.GetChar(c);
					tempo += c;
					if (c == '\'') break;
					if (!isOctal(c)) break;
				}
				if (c != '\'') {
					if (!input.EndOfInput()) {
						input.UngetString(tempo);
					}
					tmp.lexeme = "";
					tmp.token_type = ERROR;
				}
				else {
					tmp.lexeme += tempo;
					tmp.line_no = line_no;
					tmp.token_type = UNICHARACTER;
				}
			}
			else if (c == 'x') 
			{
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while(!input.EndOfInput() && i < 4 && isHexa(c))
				{	
					i++;
					input.GetChar(c);
					tempo += c;			
				}
				if (i == 2) {
					if (c == '\'') {
						tmp.lexeme += tempo;
						tmp.line_no = line_no;
						tmp.token_type = UNICHARACTER;
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
			else if (c == 'u') 
			{
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while (!input.EndOfInput() && i < 4 && isHexa(c)) 
				{
					i++;
					input.GetChar(c);
					tempo += c;
				}
				if (i == 4) 
				{
					if (c == '\'') 
					{
						tmp.lexeme += tempo;
						tmp.line_no = line_no;
						tmp.token_type = UNICHARACTER;
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
			else if (c == 'U') 
			{
				int i = 0;
				input.GetChar(c);
				tempo += c;
				while (!input.EndOfInput() && i < 8 && isHexa(c)) 
				{
					i++;
					input.GetChar(c);
					tempo += c;
				}
				if (i == 8) {
					if (c == '\'') {
						tmp.lexeme += tempo;
						tmp.line_no = line_no;
						tmp.token_type = UNICHARACTER;
					}
					else {
						if (!input.EndOfInput()) {
							input.UngetString(tempo);
						}
						tmp.lexeme = "";
						tmp.token_type = ERROR;
					}
				}
				else 
				{
					if (!input.EndOfInput()) {
						input.UngetString(tempo);
					}
					tmp.lexeme = "";
					tmp.token_type = ERROR;
				}
			}
			else if (c == '&') 
			{
				//cout << "day la debug mode\n";
				int i = 0;
				string t = "";
				while (!input.EndOfInput() && i <= 8) {
					i++;
					input.GetChar(c);
					tempo += c;
					if (c == ';') break;
					t += c;
				}
				debugg(t);
				debugg(tempo);
				debugg(to_string(c));
				if (c == ';') {
					if (IsValidEntity(t)) 
					{
						debugg("t is valid");
						input.GetChar(c);
						tempo += c;
						if (c == '\'') 
						{
							tmp.lexeme += tempo;
							tmp.line_no = line_no;
							tmp.token_type = UNICHARACTER;
						}
						else 
						{
							if (!input.EndOfInput()) {
								input.UngetString(tempo);
							}
							tmp.lexeme = "";
							tmp.token_type = ERROR;
						}
					}
					else 
					{
						if (!input.EndOfInput()) {
							input.UngetString(tempo);
						}
						tmp.lexeme = "";
						tmp.token_type = ERROR;
					}
				}
				else 
				{
					if (!input.EndOfInput()) {
						input.UngetString(tempo);
					}
					tmp.lexeme = "";
					tmp.token_type = ERROR;
				}
			}
			else 
			{
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
    if (isalpha(c) || c == '_') {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) InlineCmt
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
		case '{':
			tmp.token_type=LCURLY;
			return tmp;
		case '}':
			tmp.token_type=RCURLY;
			return tmp;
		case '.':
			tmp.token_type = DOT;
			input.GetChar(c);
			if (c == '.')
			{
				tmp.token_type = SLICE;
				input.GetChar(c);
				if (c=='.')
					tmp.token_type = DOTDOTDOT;
				return tmp;
			}
			else if (isdigit(c))
			{
				input.UngetChar(c);
				input.UngetChar('.');
				return inreal();
			}
			return tmp;
		case '+':
			input.GetChar(c);
			if (c == '+') tmp.token_type = INC;
			else if (c == '=') tmp.token_type = PLUSASSIGN;
			else 
			{
				if (!input.EndOfInput()) input.UngetChar(c);
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
			else if (c == '/') 
			{
				input.UngetChar(c);
				input.UngetChar('/');
				tmp = InlineCmt();
			}
			else if (c == '*')
			{
				input.UngetChar(c);
				input.UngetChar('/');
				tmp = BlockCmt();
			}
			else if (c == '+') 
			{
				input.UngetChar(c);
				input.UngetChar('/');
				tmp = nestedComment();
			}
			else
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = DIV;
			}
			if (tmp.token_type == ERROR) {
				input.GetChar(c);
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
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = MULT;
			}
			return tmp;
		case '=':
			input.GetChar(c);
			if (c == '=') tmp.token_type = EQUAL;
			else if (c == '>') tmp.token_type = LAMBDA;
			else
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = ASSIGN;
			}
			return tmp;
		case '!':
			input.GetChar(c);
			if (c == '=') tmp.token_type = NOTEQUAL;
			else 
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = NOT;
			}
			return tmp;
		case '&':
			input.GetChar(c);
			if (c == '&') tmp.token_type = LAND;
			else if (c == '=') tmp.token_type = ANDASSIGN;
			else 
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = BAND;
			}
			return tmp;
		case '|':
			input.GetChar(c);
			if (c == '|') tmp.token_type = LOR;
			else if (c == '=') tmp.token_type = ORASSIGN;
			else 
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = BOR;
			}
			return tmp;
		case '^':
			input.GetChar(c);
			if (c == '=') tmp.token_type = XORASSIGN;
			else if (c == '^') 
			{
				input.GetChar(c);
				if (c == '=') tmp.token_type = POWERASSIGN;
				else 
				{
					if (!input.EndOfInput()) { input.UngetChar(c); }
					tmp.token_type = POWER;
				}
			}
			else 
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
				tmp.token_type = XOR;
			}
			return tmp;
		case '~':
			input.GetChar(c);
			if (c == '=') tmp.token_type = ONECOMPLETE_ASSIGN;
			else 
			{
				if (!input.EndOfInput()) { input.UngetChar(c); }
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
			if (c == '=') 
			{
				tmp.token_type = LTEQ;
			}
			else if (c == '>') 
			{
				tmp.token_type = NOTEQUAL;
			}
			else if (c == '<') 
			{
				input.GetChar(c);
				if (c == '=') tmp.token_type = LEFTSHIFT_ASSIGN;
				else {
					if (!input.EndOfInput()) {
						input.UngetChar(c);
					}
					tmp.token_type = LEFTSHIFT;
				}
			}
			else 
			{
				if (!input.EndOfInput()) 
				{
					input.UngetChar(c);
				}
				tmp.token_type = LESS;
			}
			return tmp;
		case '>':
			input.GetChar(c);
			if (c == '=')
			{
				tmp.token_type = GTEQ;
			}
			else if (c == '>') {
				input.GetChar(c);
				if (c == '>') 
				{
					input.GetChar(c);
					if (c == '=') { tmp.token_type = LOG_RIGHTSHIFT_ASSIGN; }
					else 
					{
						if (!input.EndOfInput()) {
							input.UngetChar(c);
						}
						tmp.token_type = LOGRIGHTSHIFT;
					}
				}
				else if (c == '=') tmp.token_type = RIGHTSHIFT_ASSIGN;
				else 
				{
					if (!input.EndOfInput()) {
						input.UngetChar(c);
					}
					tmp.token_type = RIGHTSHIFT;
				}
			}
			else 
			{
				if (!input.EndOfInput()) {
					input.UngetChar(c);
				}
				tmp.token_type = GREATER;
			}
			return tmp;
		case '?':
			input.GetChar(c);
			if (c == ':') 
			{
				tmp.token_type = CONDITIONAL;
			}
			else 
			{
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
	namecharEntity = getEntity("./CharEntity.txt");
	
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

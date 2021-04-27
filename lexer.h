#ifndef __LEXER__H__
#define __LEXER__H__
#define DEBUGG

#include <vector>
#include <string>
#include "inputbuf.h"
#include "Util.h"
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <fstream>

// ------- token types -------------------
using namespace std;
typedef enum { END_OF_FILE = 0,
    //Keyword for DLANG
    ABSTRACT, ALIAS, ALIGN, ASM, ASSERT, AUTO, BODY,
    BOOL, BREAK, BYTE, CASE, CAST, CATCH, CDOUBLE,
    CENT, CFLOAT, CHAR, CLASS, CONST, CONTINUE, CREAL,
    DCHAR, DEBUG, DEFAULT, DELEGATE, DELETE,
    DEPRECATED, DO, DOUBLE, ELSE, ENUM, EXPORT, EXTERN, FALSE,
    FINAL, FINALLY, FLOAT, FOR, FOREACH, FOREACH_REVERSE,
    FUNCTION, GOTO, IDOUBLE, IF, IFLOAT, IMMUTABLE, IMPORT, IN,
    INOUT, INT, INTERFACE, INVARIANT, IREAL, IS, LAZY, LONG,
    MACRO, MIXIN, MODULE, NEW, NOTHROW, _NULL, OUT,
    OVERRIDE, PACKAGE, PRAGMA, PRIVATE, PROTECTED, PUBLIC, PURE,
    REAL, REF, RETURN, SCOPE, SHARED, SHORT, STATIC, STRUCT, SUPER,
    SWITCH, SYNCHRONIZED, TEMPLATE, THIS, THROW, TRUE, TRY, TYPEID,
    TYPEOF, UBYTE, UCENT, UINT, ULONG, UNION, UNITTEST, USHORT,
    VERSION, VOID, WCHAR, WHILE, WITH, ___FILE__, __FILE_FULL_PATH__,
    __MODULE__, ___LINE__, ___FUNCTION__, ___PRETTY_FUNCTION__,
    __GSHARED, __TRAITS, __VECTOR, __PARAMETERS,
    LCURLY, RCURLY,
    PLUS, MINUS, DIV, MULT, REMAIN, POWER,
	  INC, DEC, 
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN,
	  ASSIGN, PLUSASSIGN, MINUSASSIGN, DIVASSIGN, REMAINASSIGN, MULTASSIGN, POWERASSIGN,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ,
	  NOT, LOR, LAND,
	  BOR, BAND, XOR, ONECOMPLETE, LEFTSHIFT, RIGHTSHIFT, LOGRIGHTSHIFT,
	  ORASSIGN, ANDASSIGN, XORASSIGN, ONECOMPLETE_ASSIGN, LEFTSHIFT_ASSIGN, RIGHTSHIFT_ASSIGN,
	  LOG_RIGHTSHIFT_ASSIGN,
	  LAMBDA,
	  CONDITIONAL, QMARK,
    DOT, NUM, ID, ERROR, // TODO: Add labels for new token types here
    CHARACTER, 
    DECINT, BININT, HEXINT, OCTINT,
    WYSIWYGSTR, DOUBLESTR, DELIMITEDSTR, TOKENSTR
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();
    LexicalAnalyzer(std::istream* _stream)
    {
      input.stream = _stream;
      this->line_no = 1;
      //input.stream = &std::cin;
      tmp.lexeme = "";
      tmp.line_no = 1;
      tmp.token_type = ERROR;
    }

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool isSpecialCharacter();
    bool IsKeyword(std::string);
	  bool IsEscSequence(char);
  	bool IsValidEntity(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
	  Token ScanChar();
    Token ErrorToken();

    Token ScanString();
    Token WysString();
    Token DoubleQuoteString();
    Token DelimitedString();
    Token TokenString();
};

bool isHexa(int c);
vector<string> getEntity(string filename);

void debugg(string messages);

#endif  //__LEXER__H__

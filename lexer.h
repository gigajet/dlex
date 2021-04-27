#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

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
    //Others

    PLUS, MINUS, DIV, MULT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ,
    DOT, NUM, ID, ERROR, // TODO: Add labels for new token types here
    DECINT, BININT, HEXINT, OCTINT,
    

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

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool isSpecialCharacter();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();


    Token DecimalInteger();
    Token BinaryInteger();
    Token HexadecimalInteger();
};

#endif  //__LEXER__H__

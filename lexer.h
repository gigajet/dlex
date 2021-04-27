#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    IF, WHILE, DO, THEN, PRINT,
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
	CHAR,
    DECINT, BININT, HEXINT, OCTINT
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
    bool IsKeyword(std::string);
	bool IsEscSequence(char);
	bool IsValidEntity(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
	Token ScanChar();


    Token DecimalInteger();
    Token BinaryInteger();
    Token HexadecimalInteger();
};

#endif  //__LEXER__H__

#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"
#include "Util.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    IF, WHILE, DO, THEN, PRINT,
    PLUS, MINUS, DIV, MULT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ,
    DOT, NUM, ID, ERROR, // TODO: Add labels for new token types here
    DECINT, BININT, HEXINT, OCTINT,
    WYSIWYGSTR, DOUBLESTR, DELIMITEDSTR, TOKENSTR,
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
    // ~LexicalAnalyzer()
    // {
    //   delete input.stream;
    //   input.stream = nullptr;
    // }

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
    Token ErrorToken();

    Token ScanString();
    Token WysString();
    Token DoubleQuoteString();
    Token DelimitedString();
    Token TokenString();
};

#endif  //__LEXER__H__

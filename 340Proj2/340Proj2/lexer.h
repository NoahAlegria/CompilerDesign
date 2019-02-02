/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// Lexer modified for FIRST & FOLLOW project

typedef enum { END_OF_FILE = 0, ARROW, HASH, DOUBLEHASH, ID, ERROR } TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
    bool isTerm;
};

class Rule {
	public:

		int LHS;
		std::vector<int> RHS;
		int RHS_size;
		//Note, this may need a default constructor
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
    Token ScanId();
};

#endif  //__LEXER__H__

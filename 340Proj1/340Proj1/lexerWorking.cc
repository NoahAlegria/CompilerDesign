/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
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
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types here (DONE)
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
	bool flag1 = false;
	bool flag3 = false;
	bool flag4 = false;
	std::string numString, hexString;

    input.GetChar(c);
	if (isdigit(c)) {
		if (c == '0') {
			tmp.lexeme += c;
			input.GetChar(c); //so that it will traverse to end of file
			flag4 = true;
		}
		else {
			tmp.lexeme = "";
			while (!input.EndOfInput() && isdigit(c)) {
				tmp.lexeme += c;
				input.GetChar(c);
			}
			numString = tmp.lexeme;
			/*while (!input.EndOfInput() && isxdigit(c))
			{
				tmp.lexeme += c;
				input.GetChar(c);
				flag1 = true;
			}
			*/
		}
		// TODO: Unget Correctly
		//checks to see if the input string can be a REALNUM by identifying a DOT
		if (c == '.' && flag1 == false)
		{
			char peekedChar;
			string tempRealNum;
			bool flag2 = false;
			string tempNumString;

			tempNumString = tmp.lexeme;

			//tmp.lexeme += c; 
			//to add dot to staring
			peekedChar = input.PeekChar();
			if (isdigit(peekedChar)) //TODO: Needs to be able to handle multiple zeros after decimal
			{
				tmp.lexeme += c; // to add dot too string
				input.GetChar(c);
				while (!input.EndOfInput() && isdigit(c) && c != '\n')
				{
					tmp.lexeme += c;
					input.GetChar(c);
					tempRealNum = tmp.lexeme;
					flag2 = true;
				}
			}

			//Not a REALNUM, return tokens
			if (!isdigit(c) && c != '\n' && flag2 == false)
			{

				input.UngetChar(c);

				tmp.lexeme = tempNumString;
				tmp.token_type = NUM;
				tmp.line_no = line_no;
				return tmp;

			}
			input.UngetChar(c);
			tmp.lexeme = tempRealNum;
			tmp.token_type = REALNUM;
			tmp.line_no = line_no;
			return tmp;
		}
		//checks to see if input string can be a BASE08NUM or BASE16NUM by recognition of 'x'
		if (c == 'x' && isBase8(numString))
		{
			string tempBaseString;
			tempBaseString = tmp.lexeme;
			char peekedChar;

			peekedChar = input.PeekChar();

			if (isdigit(peekedChar))
			{
				tmp.lexeme += c;
				input.GetChar(c);
				//peekedChar = input.PeekChar();
				//Checking for BASE08NUM
				if (peekedChar == '0')
				{
					tmp.lexeme += peekedChar;
					input.GetChar(c);
					//peekedChar = input.PeekChar(); // same single value as c at this point
					if (c == '8')
					{
						tmp.lexeme += c;
						tmp.token_type = BASE08NUM;
						return tmp;
					}
					else
					{
						input.UngetChar(c);
						input.UngetChar('0');
						input.UngetChar('x');
						flag3 = true;
					}
				}
				//checking for BASE16NUM
				else if (peekedChar == '1')
				{
					tmp.lexeme += peekedChar;
					input.GetChar(c);
					if (c == '6')
					{
						tmp.lexeme += c;
						tmp.token_type = BASE16NUM;
						return tmp;
					}
					else
					{
						input.UngetChar(c);
						input.UngetChar('1');
						input.UngetChar('x');
						flag3 = true;
					}
				}
			}
			tmp.lexeme = tempBaseString;
		}
		//if defaults
		if (!input.EndOfInput() && flag3 == false)
		{
			input.UngetChar(c);
		}
		//tmp.lexeme = numString;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
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

bool LexicalAnalyzer::isBase8(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		char c = s[i];
		int act = (int)c;
		if (act < 48 || act >= 56)
		{
			return false;
		}
	}
	return true;
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

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
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

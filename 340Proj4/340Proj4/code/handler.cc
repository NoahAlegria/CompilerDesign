#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include "handler.h"
#include "lexer.h"
#include "compiler.h"

using namespace std;

LexicalAnalyzer lexer;


void syntax_error()
{
    cout << "Syntax Error\n";
    exit(1);
}

Token peek()
{
	Token t = lexer.GetToken();
	lexer.UngetToken(t);
	return t;
}

Token expect(TokenType expected_type)
{
	Token t = lexer.GetToken();
	if (t.token_type != expected_type)
		syntax_error();
	return t;
}


struct StatementNode* parse_program()
{
    //program -> var_section body
     
    Token t1 = lexer.GetToken();
    if (t1.token_type == ID)
    {
        parse_var_section();
    }
    else
    {
        syntax_error();
    }
    
    Token t2 = lexer.GetToken();
    if (t2.token_type == LBRACE)
    {
        parse_body();
    }
    else
    {
        syntax_error();
    }
}

void parse_var_section()
{

    //var_section -> id_list SEMICOLON
    Token t1 = lexer.GetToken();
    
    if (t1.token_type == ID)
        parse_id_list();
    else
        syntax_error();
        
    Token t2 = lexer.GetToken();
    
    if (t2.token_type == SEMICOLON)
    {
        //DO NOTHING; IS CORRECT
    }
    else
        syntax_error();
}

void parse_id_list()
{
    // id_list -> ID COMMA id_list
    // id_list -> ID
    
    expect(ID);
    Token t1 = peek();
    if (t1.token_type == SEMICOLON)
    {
        return;
    }
    expect(COMMA);
    parse_id_list();
}

void parse_body()
{
    // body -> LBRACE stmt_list RBRACE
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

void parse_stmt_list()
{
    // stmt_list -> stmt stmt_list
    // stmt_list -> stmt
    
    parse_stmt();
    Token t1 = peek();
    //CHECKS FIRST OF STMT
    if (t1.token_type == ID || t1.token_type == PRINT || 
        t1.token_type == WHILE || t1.token_type == IF || t1.token_type == SWITCH)
        {
            parse_stmt_list();
        }
    // CHECKS FOLLOW OF STMT_LIST
    else if (t1.token_type == RBRACE)
    {
        return; // stmt_list -> stmt
    }
    else
        syntax_error();
}

void parse_stmt()
{
    // parse_stmt -> assign_stmt
    // parse_stmt -> print_stmt
    // parse_stmt -> while_stmt
    // parse_stmt -> if_stmt
    // parse_stmt -> switch_stmt   WTF why do all these say parse? #FriedEggBrain
    Token t1 = peek();
    
    if (t1.token_type == ID)
        parse_assign_stmt();
        
    else if (t1.token_type == PRINT)
        parse_print_stmt();
    
    else if (t1.token_type == WHILE)
        parse_while_stmt();
    
    else if (t1.token_type == IF)
        parse_if_stmt();
     
    else if (t1.token_type == SWITCH)
        parse_switch_stmt();
    
    else
        syntax_error();
}

void parse_assign_stmt()
{
    // assign_stmt -> ID EQUAL primary SEMICOLON
    // assign_stmt -> ID EQUAL expr SEMICOLON
    
    Token t1 = expect(ID);
    expect(EQUAL);
    
    ValueNode *prime1 = parse_primary();
    
    Token tNot = peek();
    // checks first of op to make sure it's not just primary
    if (tNot.token_type == PLUS || tNot.token_type == MINUS ||
        tNot.token_type == MULT || tNot.token_type == DIV)
        {
            parse_expr();
        }
    expect(SEMICOLON);
    // SHIT IDK
}

void parse_expr()
{
    // expr -> primary op primary
    // CHANGING THIS TO expr -> op primary TO ACCOUNT FOR assign_stmt
    
    //parse_primary(); // may need to be altered for assign idk
    parse_op();
    parse_primary();
}

ValueNode* parse_primary()
{
    // primary -> ID
    // primary -> NUM
    
    ValueNode *newNode = new ValueNode;
    Token t1 = peek();
    
    if (t1.token_type == ID)
    {
        expect(ID);
        return newNode;
    }
        
    else if (t1.token_type == NUM)
    {
        expect(NUM);
        return newNode;
    }
    else
        syntax_error();
        
    return NULL;
}
void parse_op()
{
    // op -> PLUS
    // op -> MINUS
    // op -> MULT
    // op -> DIV
    Token t1 = lexer.GetToken();
    
    if (t1.token_type == PLUS)
        expect(PLUS);
        
    else if (t1.token_type == MINUS)
        expect(MINUS);
        
    else if (t1.token_type == MULT)
        expect(MULT);
    
    else if (t1.token_type == DIV)
        expect(DIV);
        
    else
        syntax_error();
}

void parse_print_stmt()
{
    // print_stmt -> PRINT ID SEMICOLON
    expect(PRINT);
    expect(ID);
    expect(SEMICOLON);
}

void parse_while_stmt()
{
    // while_stmt -> WHILE condition body
    
    expect(WHILE);
    parse_condition();
    parse_body();
}

void parse_if_stmt()
{
    // if_stmt -> IF condition body
    
    expect(IF);
    parse_condition();
    parse_body();
}

void parse_condition()
{
    // condition -> primary relop primary
    
    parse_primary();
    parse_relop();
    parse_primary();
}

void parse_relop()
{
    // relop -> GREATER
    // relop -> LESS
    // relop -> NOTEQUAL
    
    Token t1 = lexer.GetToken();
    
    if (t1.token_type == GREATER)
        expect(GREATER);
        
    else if (t1.token_type == LESS)
        expect(LESS);
    
    else if (t1.token_type == NOTEQUAL)
        expect(NOTEQUAL);
    
    else
        syntax_error();
}

void parse_switch_stmt()
{
    // switch_stmt -> SWITCH ID LBRACE case_list RBRACE
    // switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
    
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parse_case_list();
    
    Token t1 = peek();
    if (t1.token_type == DEFAULT)
        {
            parse_default_case();
            expect(RBRACE);
        }
    else if (t1.token_type == RBRACE)
        expect(RBRACE);
        
    else
        syntax_error();
    
}

void parse_for_stmt()
{
    // for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
    
    //expect(FOR);
    expect(LPAREN);
    parse_assign_stmt();
    parse_condition();
    expect(SEMICOLON);
    parse_assign_stmt();
    expect(RPAREN);
    parse_body();
}

void parse_case_list()
{
    // case_list -> case case_list
    // case_list -> case
    
    parse_case();
    Token t1 = peek();
    if (t1.token_type == CASE) // first of case_list
        parse_case_list();
    
    else if (t1.token_type == RBRACE || t1.token_type == DEFAULT) // follow of case_list
        return; // case_list -> case
        
    else
        syntax_error();
}

void parse_case()
{
    // case -> CASE NUM COLON body
    
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}

void parse_default_case()
{
    // default_case -> DEFAULT COLON body
    
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}
/*struct StatementNode * parse_generate_intermediate_representation()
{


}*/

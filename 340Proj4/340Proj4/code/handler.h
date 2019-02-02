#ifndef _HANDLER_H_
#define _HANDLER_H_

#include <string>
#include "lexer.h"
#include <iostream>
#include "compiler.h"


using namespace std;

StatementNode* parse_program();
void parse_var_section();
void parse_id_list();
void parse_body();
void parse_stmt_list();
void parse_stmt();
void parse_assign_stmt();
void parse_expr();
ValueNode* parse_primary();
void parse_op();
void parse_print_stmt();
void parse_while_stmt();
void parse_if_stmt();
void parse_condition();
void parse_relop();
void parse_switch_stmt();
void parse_for_stmt();
void parse_case_list();
void parse_case();
void parse_default_case();

Token expect(TokenType expected_type);
Token peek();
void syntax_error();

#endif //_HANDLER_H

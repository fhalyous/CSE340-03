/*
 * Copyright (C) Sandeep Balaji
 *
 * Do not share this file with anyone
 */

#include <vector>
#include <string>

#include "lexer.h"

class Parser{
public:
    bool parse_program();
    bool parse_scope();
    bool parse_scope_list();
    bool parse_var_decl();
    bool parse_id_list();
    bool parse_type_name();
    bool parse_stmt_list();
    bool parse_stmt();
    bool parse_assign_stmt();
    bool parse_while_stmt();
    bool parse_expr();
    bool parse_arithmetic_operator();
    bool parse_boolean_operator();
    bool parse_relational_operator();
    bool parse_primary();
    bool parse_bool_const();
    bool parse_condition();
    std::vector<Token> token_list;

private:
    LexicalAnalyzer lexer;
};

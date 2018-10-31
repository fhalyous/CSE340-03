/*
 * Copyright (C) Sandeep Balaji
 *
 * Do not share this file with anyone
 */

#include <vector>
#include <string>

#include "lexer.h"

struct Variable{
    std::string name;
    TokenType token_type;
    int line_no;
};

struct Scope{
    std::vector<Variable> vars;
    std::vector<std::string> usedVars;
    std::vector<std::string> leftVars;
    Scope* prev;
    Scope* whilescope;
};

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
    std::pair<TokenType, bool> parse_expr();
    bool parse_relational_operator();
    bool parse_primary();
    bool parse_condition();
    bool findDeclaration(std::string name);
    Token findDeclarationToken(std::string name);
    bool findInitialization(std::string name);
    void addUsedVars(std::string name);
    std::vector<Token> token_list;

private:
    LexicalAnalyzer lexer;
};

/*
 * Copyright (C) Sandeep Balaji
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <set>

#include "parser.h"

using namespace std;

string reserve[] = {"END_OF_FILE",
                    "REAL", "INT", "BOOLEAN", "STRING",
                    "WHILE", "TRUE", "FALSE", "COMMA", "COLON", "SEMICOLON",
                    "LBRACE", "RBRACE", "LPAREN", "RPAREN",
                    "EQUAL", "PLUS", "MINUS", "MULT", "DIV", "AND", "OR", "XOR", "NOT",
                    "GREATER", "GTEQ", "LESS", "LTEQ", "NOTEQUAL",
                    "ID", "NUM", "REALNUM", "STRING_CONSTANT", "ERROR"
};

Scope* scope = new Scope();
string errorCode = "";

bool Parser::parse_program() {
    if (parse_scope()) {
        Token t = lexer.GetToken();
        if (t.token_type == END_OF_FILE){
            return true;
        }
    }
    return false;
}

bool Parser::parse_scope() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == LBRACE) {
        Scope* sc = new Scope();
        sc->prev = scope;
        scope = sc;
        if (parse_scope_list()) {
            t = lexer.GetToken();
            token_list.push_back(t);
            if (t.token_type == RBRACE) {
                set<string> variableSet;
                for (auto const &vars: scope->vars){
                    if(variableSet.find(vars.name) != variableSet.end()){
                        errorCode = "ERROR CODE 1.1 " + vars.name;
                    }
                    variableSet.insert(vars.name);
                    if (find(scope->usedVars.begin(), scope->usedVars.end(), vars.name) == scope->usedVars.end()){
                        errorCode = "ERROR CODE 1.3 " + vars.name;
                    }
                    // cout << vars.name << "::" << reserve[vars.token_type] << "::" << vars.line_no << endl;
                }
                Scope* temp = scope;
                scope = scope->prev;
                free(temp);
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_scope_list() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == LBRACE) {
        lexer.UngetToken(t);
        token_list.pop_back();
        if (parse_scope()) {
            t = lexer.GetToken();
            token_list.push_back(t);
            if (t.token_type == END_OF_FILE || t.token_type == RBRACE) {
                lexer.UngetToken(t);
                token_list.pop_back();
                return true;
            } else if (t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE) {
                lexer.UngetToken(t);
                token_list.pop_back();
                if (parse_scope_list()) {
                    return true;
                }
            }
        }
    } else if (t.token_type == ID) {
        Token t1 = lexer.GetToken();
        token_list.push_back(t);
        if (t1.token_type == COLON || t1.token_type == COMMA) {
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            token_list.pop_back();
            token_list.pop_back();
            if (parse_var_decl()) {
                t = lexer.GetToken();
                token_list.push_back(t);
                if (t.token_type == RBRACE) {
                    lexer.UngetToken(t);
                    token_list.pop_back();
                    return true;
                } else if (t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE) {
                    lexer.UngetToken(t);
                    token_list.pop_back();
                    if (parse_scope_list()) {
                        return true;
                    }
                }
            }
        } else if (t1.token_type == EQUAL) {
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            token_list.pop_back();
            token_list.pop_back();
            if (parse_stmt()) {
                t = lexer.GetToken();
                token_list.push_back(t);
                if (t.token_type == RBRACE) {
                    lexer.UngetToken(t);
                    token_list.pop_back();
                    return true;
                } else if (t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE) {
                    lexer.UngetToken(t);
                    token_list.pop_back();
                    if (parse_scope_list()) {
                        return true;
                    }
                }
            }
        }
    } else if (t.token_type == WHILE) {
        lexer.UngetToken(t);
        token_list.pop_back();
        if (parse_stmt()) {
            t = lexer.GetToken();
            token_list.push_back(t);
            if (t.token_type == RBRACE) {
                lexer.UngetToken(t);
                token_list.pop_back();
                return true;
            } else if (t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE) {
                lexer.UngetToken(t);
                token_list.pop_back();
                if (parse_scope_list()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_var_decl() {
    if (parse_id_list()) {
        Token t = lexer.GetToken();
        token_list.push_back(t);
        if (t.token_type == COLON) {
            t = lexer.GetToken();
            vector<Variable>::iterator i;
            for (i = scope->vars.begin(); i != scope->vars.end(); i++){
                if (i->line_no == t.line_no){
                    i->token_type = t.token_type;
                }
            }
            lexer.UngetToken(t);
            if (parse_type_name()) {
                t = lexer.GetToken();
                token_list.push_back(t);
                if (t.token_type == SEMICOLON) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_id_list() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == ID) {
        Variable variable;
        variable.name = t.lexeme;
        variable.line_no = t.line_no;
        scope->vars.push_back(variable);
        t = lexer.GetToken();
        token_list.push_back(t);
        if (t.token_type == COLON) {
            lexer.UngetToken(t);
            token_list.pop_back();
            return true;
        } else if (t.token_type == COMMA) {
            if (parse_id_list()) {
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_type_name() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == REAL || t.token_type == INT || t.token_type == BOOLEAN || t.token_type == STRING) {
        return true;
    }
    return false;
}

bool Parser::parse_stmt_list() {
    if (parse_stmt()) {
        Token t = lexer.GetToken();
        token_list.push_back(t);
        if (t.token_type == LBRACE || t.token_type == RBRACE) {
            lexer.UngetToken(t);
            token_list.pop_back();
            return true;
        } else if (t.token_type == ID || t.token_type == WHILE) {
            lexer.UngetToken(t);
            token_list.pop_back();
            if (parse_stmt_list()) {
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_stmt() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == ID) {
        lexer.UngetToken(t);
        token_list.pop_back();
        if (parse_assign_stmt()) {
            return true;
        }
    } else if (t.token_type == WHILE) {
        lexer.UngetToken(t);
        token_list.pop_back();
        if (parse_while_stmt()) {
            return true;
        }
    }
    return false;
}

bool Parser::parse_assign_stmt() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == ID) {
        addUsedVars(t.lexeme);
        scope->leftVars.push_back(t.lexeme);
        if(!findDeclaration(t.lexeme)){
            errorCode = "ERROR CODE 1.2 " + t.lexeme;
        }
        Token tt = t;
        t = lexer.GetToken();
        token_list.push_back(t);
        if (t.token_type == EQUAL) {
            pair<TokenType, bool> p = parse_expr();
            if (tt.token_type != REAL){
                if (tt.token_type != p.first){
                    errorCode = "TYPE MISMATCH " + to_string(tt.line_no) + " C1";
                    cout << errorCode << endl;
                }
            }
            else if (tt.token_type == REAL){
                if (p.first != INT || p.first != REAL){
                    errorCode = "TYPE MISMATCH " + to_string(tt.line_no) + " C2";
                    cout << errorCode << endl;
                }
            }
            if (p.second) {
                t = lexer.GetToken();
                token_list.push_back(t);
                if (t.token_type == SEMICOLON) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_while_stmt() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == WHILE) {
        if (parse_condition()) {
            t = lexer.GetToken();
            token_list.push_back(t);
            if (t.token_type == LBRACE) {
                if (parse_stmt_list()) {
                    t = lexer.GetToken();
                    token_list.push_back(t);
                    if (t.token_type == RBRACE) {
                        return true;
                    }
                }
            } else if (t.token_type == ID || t.token_type == WHILE) {
                lexer.UngetToken(t);
                token_list.pop_back();
                if (parse_stmt()) {
                    return true;
                }
            }
        }
    }
    return false;
}

pair<TokenType, bool> Parser::parse_expr() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT ) {
        pair<TokenType, bool> t1 = parse_expr();
        pair<TokenType, bool> t2 = parse_expr();
        TokenType tt = ERROR;
        if ((t1.first == REAL && t2.first == INT )|| (t1.first == INT && t2.first == REAL)){
            tt = REAL;
        }
        else if (t1.first == INT && t2.first == INT){
            tt = INT;
        }
        if (t1.second && t2.second){
            return make_pair(tt, true);
        }
    }else if (t.token_type == DIV){
        pair<TokenType, bool> t1 = parse_expr();
        pair<TokenType, bool> t2 = parse_expr();
        TokenType tt = ERROR;
        if ((t1.first == REAL && t2.first == INT )|| (t1.first == INT && t2.first == REAL)){
            tt = REAL;
        }
        else if (t1.first == INT && t2.first == INT){
            tt = REAL;
        }
        if (t1.second && t2.second){
            return make_pair(tt, true);
        }
    } else if (t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS
               || t.token_type == NOTEQUAL || t.token_type == LTEQ) {
        pair<TokenType, bool> t1 = parse_expr();
        pair<TokenType, bool> t2 = parse_expr();
        TokenType tt = ERROR;
        if (t1.first == BOOLEAN && t2.first == BOOLEAN){
            tt = BOOLEAN;
        }
        else if (t1.first == STRING && t2.first == STRING){
            tt = BOOLEAN;
        }
        else if ((t1.first == REAL && t2.first == INT )|| (t1.first == INT && t2.first == REAL)){
            tt = BOOLEAN;
        }
        else if (t1.first == INT && t2.first == INT){
            tt = BOOLEAN;
        }
        if (t1.second && t2.second){
            return make_pair(tt, true);
        }
    } else if (t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM
               || t.token_type == STRING_CONSTANT || t.token_type == TRUE || t.token_type == FALSE) {
        Token t1 = t;
        lexer.UngetToken(t);
        token_list.pop_back();
        if (parse_primary()) {
            //Uncomment this
            // if (t.token_type == ID){
            //     return findDeclaration1(t1.lexeme);
            // }
            return make_pair(t1.token_type, true);
        }
    } else if (t.token_type == AND || t.token_type == OR || t.token_type == XOR) {
        pair<TokenType, bool> t1 = parse_expr();
        pair<TokenType, bool> t2 = parse_expr();
        TokenType tt = ERROR;
        if (t1.first == BOOLEAN && t2.first == BOOLEAN){
            tt = BOOLEAN;
        }
        if (t1.second && t2.second){
            return make_pair(tt, true);
        }
    } else if (t.token_type == NOT) {
        return parse_expr();
    }
    return make_pair(ERROR, false);
}

bool Parser::parse_primary() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == ID){
        scope->usedVars.push_back(t.lexeme);
        if(!findInitialization(t.lexeme)){
            errorCode = errorCode + "UNINITIALIZED " + t.lexeme + " " + to_string(t.line_no) + "\n";
        }
        if(!findDeclaration(t.lexeme)){
            errorCode = "ERROR CODE 1.2 " + t.lexeme;
        }
    }
    if (t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM || t.token_type == STRING_CONSTANT) {
        return true;
    } else if (t.token_type == TRUE || t.token_type == FALSE) {
        return true;
    }
    return false;
}

bool Parser::parse_condition() {
    Token t = lexer.GetToken();
    token_list.push_back(t);
    if (t.token_type == LPAREN) {
        if (parse_expr().second) {
            t = lexer.GetToken();
            token_list.push_back(t);
            if (t.token_type == RPAREN) {
                return true;
            }
        }
    }
    return false;
}

bool Parser::findDeclaration(string name){
    vector<Variable>::iterator i;
    bool found = false;
    Scope* foo = scope;
    while(foo != NULL){
        for (i = foo->vars.begin(); i != foo->vars.end(); i++){
            if (name == i->name){
                found = true;
            }
        }
        foo = foo->prev;
    }
    return found;
}

TokenType Parser::findDeclaration1(string name){
    vector<Variable>::iterator i;
    TokenType found = ERROR;
    Scope* foo = scope;
    while(foo != NULL){
        for (i = foo->vars.begin(); i != foo->vars.end(); i++){
            if (name == i->name){
                found = i->token_type;
            }
        }
        foo = foo->prev;
    }
    return found;
}

bool Parser::findInitialization(string name){
    bool found = false;
    Scope* foo = scope;
    while(foo != NULL){
        if (find(foo->leftVars.begin(), foo->leftVars.end(), name) != foo->leftVars.end()){
            found = true;
        }
        foo = foo->prev;
    }
    return found;
}

void Parser::addUsedVars(string name){
    Scope* foo = scope;
    while(foo != NULL){
        foo->usedVars.push_back(name);
        foo = foo->prev;
    }
}

int main() {
    Parser parser;
    if (parser.parse_program()) {
        cout << errorCode << endl;
    } else {
        cout << "Syntax Error" << endl;
    }
}

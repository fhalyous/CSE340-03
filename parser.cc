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

#include "parser.h"

using namespace std;

string reserve[] = { "END_OF_FILE",
    "REAL", "INT", "BOOLEAN", "STRING",
    "WHILE", "TRUE", "FALSE", "COMMA", "COLON", "SEMICOLON",
    "LBRACE", "RBRACE", "LPAREN", "RPAREN",
    "EQUAL", "PLUS", "MINUS", "MULT", "DIV","AND", "OR", "XOR", "NOT",
    "GREATER", "GTEQ", "LESS", "LTEQ", "NOTEQUAL",
    "ID", "NUM", "REALNUM", "STRING_CONSTANT", "ERROR"
};

bool Parser::parse_program(){
    if(parse_scope()){
        return true;
    }
    return false;
}

bool Parser::parse_scope(){
    Token t = lexer.GetToken();
    if (t.token_type == LBRACE){
        if (parse_scope_list()){
            t = lexer.GetToken();
            if (t.token_type == RBRACE){
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_scope_list(){
    Token t = lexer.GetToken();
    if (t.token_type == LBRACE){
        lexer.UngetToken(t);
        if (parse_scope()){
            t = lexer.GetToken();
            if (t.token_type == END_OF_FILE || t.token_type == RBRACE){
                lexer.UngetToken(t);
                return true;
            }
            else if(t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE ){
                lexer.UngetToken(t);
                if (parse_scope_list()){
                    return true;
                }
            }
        }
    }
    else if (t.token_type == ID){
        Token t1 = lexer.GetToken();
        if (t1.token_type == COLON || t1.token_type == COMMA){
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            if (parse_var_decl()){
                t = lexer.GetToken();
                if (t.token_type == RBRACE){
                    lexer.UngetToken(t);
                    return true;
                }
                else if(t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE ){
                    lexer.UngetToken(t);
                    if (parse_scope_list()){
                        return true;
                    }
                }
            }
        }
        else if (t1.token_type == EQUAL){
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            if (parse_stmt()){
                t = lexer.GetToken();
                if (t.token_type == RBRACE){
                    lexer.UngetToken(t);
                    return true;
                }
                else if(t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE ){
                    lexer.UngetToken(t);
                    if (parse_scope_list()){
                        return true;
                    }
                }
            }
        }
    }
    else if (t.token_type == WHILE){
        lexer.UngetToken(t);
        if (parse_stmt()){
            t = lexer.GetToken();
            if (t.token_type == RBRACE){
                lexer.UngetToken(t);
                return true;
            }
            else if (t.token_type == LBRACE || t.token_type == ID || t.token_type == WHILE ){
                lexer.UngetToken(t);
                if (parse_scope_list()){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_var_decl(){
    if (parse_id_list()){
        Token t = lexer.GetToken();
        if (t.token_type == COLON){
            if (parse_type_name()){
                t = lexer.GetToken();
                if (t.token_type == SEMICOLON){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_id_list(){
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        t = lexer.GetToken();
        if (t.token_type == COLON){
            lexer.UngetToken(t);
            return true;
        }
        else if (t.token_type == COMMA){
            if (parse_id_list()){
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_type_name(){
    Token t = lexer.GetToken();
    if (t.token_type == REAL || t.token_type == INT || t.token_type == BOOLEAN || t.token_type ==STRING){
        return true;
    }
    return false;
}

bool Parser::parse_stmt_list(){
    if (parse_stmt()){
        Token t = lexer.GetToken();
        if (t.token_type == LBRACE || t.token_type == RBRACE){
            lexer.UngetToken(t);
            return true;
        }
        else if (t.token_type == ID || t.token_type == WHILE){
            lexer.UngetToken(t);
            if (parse_stmt_list()){
                return true;
            }
        }
    }
    return false;
}

bool Parser::parse_stmt(){
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        lexer.UngetToken(t);
        if (parse_assign_stmt()){
            return true;
        }
    }
    else if (t.token_type == WHILE){
        lexer.UngetToken(t);
        if (parse_while_stmt()){
            return true;
        }
    }
    return false;
}

bool Parser::parse_assign_stmt(){
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        t = lexer.GetToken();
        if (t.token_type == EQUAL){
            if(parse_expr()){
                t = lexer.GetToken();
                if (t.token_type == SEMICOLON){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_while_stmt(){
    Token t = lexer.GetToken();
    if (t.token_type == WHILE){
        if(parse_condition()){
            t = lexer.GetToken();
            if (t.token_type == LBRACE){
                if (parse_stmt_list()){
                    t = lexer.GetToken();
                    if (t.token_type == RBRACE){
                        return true;
                    }
                }
            }
            else if (t.token_type == ID || t.token_type == WHILE){
                lexer.UngetToken(t);
                if (parse_stmt()){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Parser::parse_expr(){
    Token t = lexer.GetToken();
    if (t.token_type == PLUS || t.token_type ==  MINUS|| t.token_type == MULT || t.token_type == DIV){
        lexer.UngetToken(t);
        if (parse_arithmetic_operator()){
            if (parse_expr()){
                if (parse_expr()){
                    return true;
                }
            }
        }
    }
    else if (t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS
        ||t.token_type ==  NOTEQUAL ||t.token_type ==  LTEQ){
            lexer.UngetToken(t);
            if (parse_relational_operator()){
                if (parse_expr()){
                    if (parse_expr()){
                        return true;
                    }
                }
            }
    }
    else if (t.token_type == ID || t.token_type ==  NUM|| t.token_type == REALNUM
        || t.token_type == STRING_CONSTANT ||t.token_type == TRUE || t.token_type ==  FALSE){
        lexer.UngetToken(t);
        if (parse_primary()){
            return true;
        }
    }
    else if (t.token_type == AND || t.token_type ==  OR|| t.token_type == XOR){
        lexer.UngetToken(t);
        if (parse_boolean_operator()){
            if (parse_expr()){
                if (parse_expr()){
                    return true;
                }
            }
        }
    else if (t.token_type == NOT){
        if (parse_expr()){
            return true;
        }
    }
    return false;
}

bool Parser::parse_arithmetic_operator(){
    Token t = lexer.GetToken();
    if (t.token_type == PLUS || t.token_type ==  MINUS|| t.token_type == MULT || t.token_type == DIV){
        return true;
    }
    return false;
}

bool Parser::parse_boolean_operator(){
    Token t = lexer.GetToken();
    if (t.token_type == AND || t.token_type ==  OR|| t.token_type == XOR){
        return true;
    }
    return false;
}

bool Parser::parse_relational_operator(){
    Token t = lexer.GetToken();
    if (t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS
        ||t.token_type ==  NOTEQUAL ||t.token_type ==  LTEQ){
            return true;
    }
    return false;
}

bool Parser::parse_primary(){
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type ==  NUM|| t.token_type == REALNUM || t.token_type == STRING_CONSTANT){
        return true;
    }
    else if (t.token_type == TRUE || t.token_type ==  FALSE){
        return true;
    }
    return false;
}

bool Parser::parse_bool_const(){
    Token t = lexer.GetToken();
    if (t.token_type == TRUE || t.token_type ==  FALSE){
        return true;
    }
    return false;
}

bool Parser::parse_condition(){
    Token t = lexer.GetToken();
    if (t.token_type == LPAREN){
        if (parse_expr()){
            t = lexer.GetToken();
            if (t.token_type == RPAREN){
                return true;
            }
        }
    }
    return false;
}

int main()
{
    LexicalAnalyzer lexer;
    Parser parser;
    if (parser.parse_program()){
        cout << "No Syntax Error" << endl;
    }else{
        cout << "Syntax Error" << endl;
    }

}

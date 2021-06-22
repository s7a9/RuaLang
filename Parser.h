#ifndef H_PARSER
#define H_PARSER

#define TOK_VAR     0
#define TOK_OPE     1
#define TOK_CF      2
#define TOK_END     3

#include <string>
#include "Variables.h"
#include "Operators.h"

struct TokenInfo {
    int type, info;
    std::string name, text;

    TokenInfo(int t, int i, std::string n, std::string tt);

    TokenInfo();
};

class RuaParser {
private:
    const TokenInfo g_internal_tokens[50] = {
        {TOK_CF, WHILE, "WHILE", "while"},
        {TOK_CF, FOR, "FOR", "for"},
        {TOK_CF, IF, "IF", "if"},
        {TOK_CF, ELIF, "ELIF", "elif"},
        {TOK_CF, ELSE, "ELSE", "else"},
        {TOK_OPE, RETURN, "RETURN", "return"},
        {TOK_OPE, BREAK, "BREAK", "break"},
        {TOK_OPE, CONTINUE, "CONTINUE", "continue"},

        {TOK_OPE, '+', "__add__", "+"},
        {TOK_OPE, '-', "__minus__", "-"},
        {TOK_OPE, '*', "__multiple", "*"},
        {TOK_OPE, '/', "__divide__", "/"},
        {TOK_OPE, '\\', "__int_divide__", "\\"},
        {TOK_OPE, OPE_MOVE_LEFT, "__move_left__", "<<"},
        {TOK_OPE, OPE_MOVE_RIGHT, "__move_right__", ">>"},
        {TOK_OPE, '<', "__less__", "<"},
        {TOK_OPE, OPE_EQUAL, "__equal__", "=="},
        {TOK_OPE, '>', "__greater__", ">"},
        {TOK_OPE, OPE_LEQ, "__leq__", "<="},
        {TOK_OPE, OPE_GEQ, "__geq__", ">="},
        {TOK_OPE, OPE_NEQ, "__neq__", "!="},
        {TOK_OPE, '(', "left bracket", "("},
        {TOK_OPE, ')', "right bracket", ")"},
        {TOK_OPE, '=', "__give_value__", "="},
        {TOK_OPE, '&', "__and__", "&"},
        {TOK_OPE, '.', "__DOT__", "."},
        {TOK_OPE, ';', "END", ";"},
        {TOK_OPE, '[', "left List", "["},
        {TOK_OPE, ']', "right List", "]"},
        {TOK_OPE, '{', "left block", "{"},
        {TOK_OPE, '}', "right block", "}"},
        {TOK_OPE, ',', "comma", ","},
        {TOK_OPE, OPE_NEG, "__nega__", "(nega)"},
        {TOK_OPE, OPE_INDEX, "__index__", "(index)"},
        {TOK_OPE, '%', "__mod__", "%"},
        {TOK_OPE, OPE_AND, "__and__", "&&"},
        {TOK_OPE, OPE_OR, "__or__", "||"},
        {TOK_OPE, OPE_DEL, "delete", "del"},
        {TOK_OPE, OPE_CALL, "call function", "(call)"},

        {TOK_VAR, VAR_FUNC, "FUNC", "function"},
        {TOK_VAR, VAR_CLASS, "CLASS", "class"},
        {TOK_VAR, VAR_TRUE, "true", "true"},
        {TOK_VAR, VAR_FALSE, "false", "false"},
    };

    const int g_n_internal_token = sizeof(g_internal_tokens) / sizeof(TokenInfo);

    std::string m_program_text;

    int m_lineno, m_charno, idx, m_nxt_new_idx = VAR_NEW;

    std::map<int, TokenInfo> m_tokenInfo;

    std::map<std::string, int> m_name2ID;

public:
    RuaParser();

    int NewToken(int type, std::string text, std::string name);

    TokenInfo Parse(RuaData& data);

    void PutText(std::string program_text);

    std::string GetPosition();

    TokenInfo* GetTokenInfo(int id);

    TokenInfo* GetTokenInfo(std::string name);

    ~RuaParser();
};

#endif //H_PARSER
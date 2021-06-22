#include "Parser.h"

#include <math.h>
#include <cctype>
#include <cstring>

#define C (m_program_text[idx])

TokenInfo::TokenInfo(int t, int i, std::string n, std::string tt) {
    type = t, info = i, name = n, text = tt;
}

TokenInfo::TokenInfo() {
    type = 0, info = 0, name = "", text = "";
}

RuaParser::RuaParser() {
    m_lineno = 1, m_charno = idx = 0;

    for (int i = 0; i < g_n_internal_token && g_internal_tokens[i].info; i++) {
        m_tokenInfo.insert(std::make_pair(g_internal_tokens[i].info, g_internal_tokens[i]));
        m_name2ID.insert(std::make_pair(g_internal_tokens[i].text, g_internal_tokens[i].info));
        //m_tokenInfo[g_internal_tokens[i].info] = g_internal_tokens[i];
        //m_name2ID[g_internal_tokens[i].text] = g_internal_tokens[i].info;
    }
}

int RuaParser::NewToken(int type, std::string text, std::string name)
{
    TokenInfo ti(type, m_nxt_new_idx++, name, text);
    m_name2ID.insert(std::make_pair(ti.text, ti.info));
    m_tokenInfo.insert(std::make_pair(ti.info, ti));
    return m_nxt_new_idx - 1;
}

void RuaParser::PutText(std::string program_text) {
    m_program_text = program_text;
    idx =  m_charno = 0; m_lineno = 1;
}

TokenInfo RuaParser::Parse(RuaData& data) {
    std::string token_text;
    data = RuaData{0};
    int tmp;
    static bool last_is_ope = true;
    while (C != '\0') {
        if (C == '\n') m_charno = idx, m_lineno ++, idx++;
        else if (isblank(C)) idx++;
        else if (isdigit(C) || (last_is_ope && C == '-')) {
            if (C == '-') {
                token_text = '-';
                last_is_ope = true; idx++;
                if (!isdigit(C)) return m_tokenInfo[OPE_NEG];
            } else last_is_ope = false;
            do {
                data.i = data.i * 10 - '0' + C, token_text += C;
                idx++;
            } while (isdigit(C));
            if (C == '.'){
                data.d = (ruaFloat)data.i, tmp = 0, idx++, token_text += '.';
                while (isdigit(C)) {
                    data.d = data.d * 10.0 - '0' + C, token_text += C;
                    idx++, tmp++;
                }
                data.d = data.d / pow(10, tmp) * (last_is_ope ? -1 : 1);
                //printf("DATA: %f   ", data.d);
                return TokenInfo(TOK_VAR, VAR_FLOAT, "CONST", token_text);
            }
            else {
                data.i *=  (last_is_ope ? -1 : 1);
                //printf("DATA: %d   ", data.i);
                return TokenInfo(TOK_VAR, VAR_INTEAGER, "CONST", token_text);
            }
        } 
        else if (isalpha(C) || C == '_') {
            last_is_ope = false;
            do
            {
                token_text += C; idx++;
            } while (isalnum(C) || C == '_');
            auto iter = m_name2ID.find(token_text);
            if (iter == m_name2ID.end()) {
                TokenInfo ti = TokenInfo(TOK_VAR, m_nxt_new_idx, 
                                         "New Variable", token_text);
                m_name2ID[token_text] = m_nxt_new_idx;
                m_tokenInfo[m_nxt_new_idx ++] = ti;
                return ti;
            }
            else {
                last_is_ope = m_tokenInfo[iter->second].name != "New Variable";
                return m_tokenInfo[iter->second];
            }
        }
        else {
            if (C == '#') {
                do
                {
                    idx++;
                } while (C != '\0' && C != '\n');
                continue;
            }
            if (C == '[' && !last_is_ope) {
                last_is_ope = true; idx++;
                return m_tokenInfo[OPE_INDEX];
            }
            if (C == '(' && !last_is_ope) {
                last_is_ope = true; idx++;
                return m_tokenInfo[OPE_CALL];
            }
            if (C == '\"') {
                idx++;
                while (C != '\"')
                {
                    if (C == '\\') {
                        idx++;
                        switch (C)
                        {
                        case 't':
                            token_text += '\t'; break;
                        case 'n':
                            token_text += '\n'; break;
                        case '\\':
                            token_text += '\\'; break;
                        case '"':
                            token_text += '"'; break;
                        default:
                            EasyLog::Write("Warning (Parser): No " + C + 
                                           std::string(" after '\\'."));
                            break;
                        }
                    } else token_text += C;
                    idx++;
                }
                idx++;
                data.s = new std::string(token_text);
                return TokenInfo(TOK_VAR, VAR_STRING, "string", token_text);
            }
            last_is_ope = true;
            std::map<std::string, int>::iterator iter;
            do
            {
                token_text += C; idx++;
            } while (!isalnum(C) && !isblank(C) && C);
            iter = m_name2ID.find(token_text);
            if (iter == m_name2ID.end()) {
                do
                {
                    idx--, token_text = token_text.substr(0, token_text.size() - 1);
                    iter = m_name2ID.find(token_text);
                } while (iter == m_name2ID.end() && token_text.size() >= 1);
                if (iter == m_name2ID.end()) {
                    EasyLog::Write("Error (Parser): Unexpexted \" " + token_text +
                        " \" appears in program text.");
                    return TokenInfo(-1, 0, "error", token_text);
                }
            }
            return m_tokenInfo[iter->second];
        }
    }
    return TokenInfo(TOK_END, 0, "", "");
}

std::string RuaParser::GetPosition() {
    return "(" + std::to_string(m_lineno) + ", " + std::to_string(idx - m_charno) + ")";
}

TokenInfo* RuaParser::GetTokenInfo(int id) {
    auto iter = m_tokenInfo.find(id);
    if (iter == m_tokenInfo.end()) {
        EasyLog::Write("Warning (Parser): Search token failed : " + 
                       std::to_string(id));
        return nullptr;
    }
    return &(iter->second);
}

TokenInfo* RuaParser::GetTokenInfo(std::string name) {
    auto iter = m_name2ID.find(name);
    if (iter == m_name2ID.end()) {
        EasyLog::Write("Warning (Parser): Search token failed : " + name);
        return nullptr;
    }
    return &m_tokenInfo[iter->second];
}

RuaParser::~RuaParser() {
}
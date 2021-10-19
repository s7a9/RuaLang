#include "Variables.h"

RuaVariable::RuaVariable(RuaVarType t, RuaData d, int r) {
    type = t, data = d, refCnt = r;
}

RuaVarManager::RuaVarManager()
{
    m_maxIdx = 1;
    m_vars.push_back(nullptr);
}

uint RuaVarManager::CopyVar(uint id) {
    RuaVariable* rv = GetVar(id);
    if (rv == nullptr) return 0;
    RuaData rd;
    switch (rv->type) {
    case Boolean: case Integer: case Float:
        rd = rv->data; break;
    case String:
        rd.s = new ruaString(*rv->data.s);break;
    case List:
        rd.l = new ruaList(*rv->data.l);break;
    case Class:
        EasyLog::Write("Error (VarManager): trying to copy class."); 
        return 0;
    case Function:
        rd.f = new RuaFuncInfo(*rv->data.f); break;
    default:
        return 0;
    }
    return AllocateVar(rv->type, false, rd);
}

uint RuaVarManager::AllocateVar(RuaVarType type, bool isConstant) {
    RuaData data;
    switch (type)
    {
    case Boolean:
        data.b = false;
        return AllocateVar(type, isConstant, data);
    case Integer:
        data.i = 0;
        return AllocateVar(type, isConstant, data);
    case Float:
        data.d = .0;
        return AllocateVar(type, isConstant, data);
    case String:
        data.s = new ruaString;
        return AllocateVar(type, isConstant, data);
    case List:
        data.l = new ruaList;
        return AllocateVar(type, isConstant, data);
    case Class:
        data.c = new ruaCLass;
        return AllocateVar(type, isConstant, data);
    case Function:
        EasyLog::Write("Error (VarManager): trying to allocate function without initial value!"); 
        return -1;
    default:
        return -1;
    }
}

uint RuaVarManager::AllocateVar(RuaVarType type, bool isConstant, RuaData data) {
    RuaVariable* pvar = new RuaVariable(type, data, isConstant ? -0x5f7f7f7f : 1);
    /*while (m_vars.find(m_maxIdx) != m_vars.end() || m_maxIdx == 0) m_maxIdx ++;
    m_vars[m_maxIdx++] = pvar;
    return m_maxIdx - 1;*/
    uint id;
    if (m_used_vars.empty()) {
        if (m_maxIdx == 0) {
            EasyLog::Write("Error (VarManager): allocated too many variables!");
            exit(-1);
        }
        id = m_maxIdx;
        m_maxIdx++;
        m_vars.push_back(pvar);
    }
    else {
        id = m_used_vars.front();
        m_used_vars.pop_front();
        m_vars[id] = pvar;
    }
    return id;
}

RuaVariable* RuaVarManager::GetVar(uint idx) {
    if (idx >= m_vars.size()) return nullptr;
    return m_vars[idx];
}

void RuaVarManager::UnrefVar(uint idx) {
    auto pvar = GetVar(idx);
    if (pvar == nullptr) return;
    pvar->refCnt --;
    if (pvar->refCnt == 0) {
        DeleteVar(idx);
    }
}

void RuaVarManager::DeleteVar(uint idx) {
    auto pvar = GetVar(idx);
    switch (pvar->type)
    {
    case String:
        delete pvar->data.s; break;
    case List:
        delete pvar->data.l; break;
    case Class:
        delete pvar->data.c; break;
    case Function:
        delete pvar->data.f; break;
    default:
        break;
    }   
    delete pvar;
    m_vars[idx] = nullptr;
    m_used_vars.push_back(idx);
}

void RuaVarManager::RefVar(uint idx) {
    auto pvar = GetVar(idx);
    if (pvar == nullptr) return;
    pvar->refCnt ++;
}

std::string RuaVarManager::to_string(uint id, bool full) {
    auto p = GetVar(id);
    if (p == nullptr) return "NULL";
    std::string ret = full ? ("{ " + std::to_string(id) + " : ") : "";
    switch (p->type)
    {
    case Boolean:
        ret += p->data.b ? "true" : "false";
        break;
    case Integer:
        ret += std::to_string(p->data.i);
        break;
    case Float:
        ret += std::to_string(p->data.d);
        break;
    case String:
        if (full)
            ret += '"' + *p->data.s + '"';
        else
            ret += *p->data.s;
        break;
    case List:
        ret += "[ list";
        //for (auto i : *p->data.l) ret += to_string(i) + ", ";
        ret += ']';
        break;
    case Class:
        ret += "Class";
        break;
    case Function:
        ret += "Function";
        break;
    default:
        break;
    }
    if (full)
        ret += " }";
    return ret;
}
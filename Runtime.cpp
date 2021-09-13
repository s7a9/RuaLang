#include "Runtime.h"

#include "Calculator.h"

#define BEGIN_OF_TEMP_VAR  (0x5ffff)

#define CHKRET(F) if (R=(F)) return R;

#define TOPENV (m_env_stk.top())

#define POPTOP if (HIDWORD(TOPENV.varstk.top()) == 0) UnrefVar(TOPENV.varstk.top()); TOPENV.varstk.pop();

#define GETCHK(tok) if((m_ti=m_parser.Parse(m_tmp)).info!=tok)\
                {EasyLog::Write("Preprocess (Error): in position " + \
                m_parser.GetPosition() + " expectd " + m_parser.GetTokenInfo(tok)->text + \
                " but found " + m_parser.GetTokenInfo(m_ti.info)->text); exit(0);}

#define CHK(tok) if(m_ti.info!=tok)\
                {EasyLog::Write("Preprocess (Error): in position " + \
                m_parser.GetPosition() + " expectd " + m_parser.GetTokenInfo(tok)->text + \
                " but found " + m_parser.GetTokenInfo(m_ti.info)->text); exit(0);}

#define NXT m_ti = m_parser.Parse(m_tmp);

// #define Write(str) Write('[' + TOPENV.name + ']' + str)

RuaSentence RuaRuntime::parseExpression(int prio) {
    //printf("%s %d %s %d\n", m_parser.GetPosition().c_str(), m_ti.info, m_ti.text.c_str(), prio);
    RuaSentence sent, sttmp;
    std::stack<RuaCommand> stk;
    while (true) {
        //printf("!! %d %s\n", m_ti.info, m_ti.text.c_str());
        if (m_ti.info == '}' || m_ti.type == TOK_END) {
            popStack(sent, stk);
            return sent;
        }
        if (m_ti.info == ';') {
            popStack(sent, stk);
            if (prio == 0) NXT
            return sent;
        }
        //if ((m_ti.info == ',' || m_ti.info == ']' || m_ti.info == ')') && prio == 2) {
        if ((m_ti.info == ',' || m_ti.info == ']') && prio == 2) {
            popStack(sent, stk);
            return sent;
        }
        if (m_ti.info == ')') {
            while (!stk.empty() && stk.top().cmd != '(') {
                sent.push_back(stk.top());
                stk.pop();
            }
            if (stk.empty()) return sent;
            stk.pop();
            NXT
        }
        else if (m_ti.info == VAR_CLASS) {
            GETCHK('{') NXT
            while (m_ti.type == TOK_VAR) {
                if (m_ti.info < VAR_NEW) {
                    EasyLog::Write("Preprocess (Error): in position " + m_parser.GetPosition() +
                        " class member must be variable but " + m_ti.text);
                    exit(0);
                }
                GETCHK('=')
                sttmp = parseExpression(2);

            }
        }
        else if (m_ti.info == VAR_FUNC) {
            GETCHK('(') popStack(sent, stk); NXT
            RuaFuncInfo* finfo = new RuaFuncInfo;
            finfo->func_type = FUNC_GLOBAL;
            while (m_ti.info != ')') {
                if (m_ti.name != "New Variable") {
                    EasyLog::Write("Preprocess (Error): in position " + m_parser.GetPosition() + 
                    " expectd variable but found " + m_ti.text); 
                    exit(0);
                }
                finfo->paras.push_back(m_ti.info); NXT
                if (m_ti.info == ',') NXT
            }
            GETCHK('{')
            finfo->data = new RuaControlFLow(parseControlFlow());
            m_parser.GetLastIsOpe() = true;
            m_tmp.f = finfo;
            //printf("FUNC %d\n { ", finfo->paras.size());
            //static_cast<RuaControlFLow*>(finfo->data)->Print();
            //printf(" } \n");
            stk.push({OPE_PUSH_CONST, m_vm.AllocateVar(Function, true, m_tmp)});
        }
        else if (m_ti.info == '[') {
            sent.push_back({'[', 0});
            NXT
            while (m_ti.info != ']') {
                sttmp = parseExpression(2);
                sent.insert(sent.end(), sttmp.begin(), sttmp.end());
                if (m_ti.info == ',') {
                    sent.push_back({',', 0});
                    NXT
                }
            }
            sent.push_back({']', 0}); NXT
        }
        else if (m_ti.info == OPE_INDEX) {
            popStack(sent, stk);
            NXT;
            sttmp = parseExpression(2);
            sent.insert(sent.end(), sttmp.begin(), sttmp.end());
            sent.push_back({ OPE_INDEX, 0 });
            CHK(']')
            m_parser.GetLastIsOpe() = false;
            NXT
        }
        else if (m_ti.info == OPE_CALL) {
            popStack(sent, stk);
            sent.push_back({'[', 0});
            NXT
            while (m_ti.info != ')') {
                sttmp = parseExpression(2);
                sent.insert(sent.end(), sttmp.begin(), sttmp.end());
                if (m_ti.info == ',') {
                    sent.push_back({',', 0});
                    NXT
                }
            }
            sent.push_back({']', 0});
            sent.push_back({OPE_CALL, 0});
            NXT
        }
        else {
            popStack(sent, stk);
            uint id;
            if (m_ti.type == TOK_VAR) {
                switch (m_ti.info)
                {
                case VAR_TRUE: case VAR_FALSE:
                    m_tmp.b = "true" == m_ti.text;
                    id = m_vm.AllocateVar(Boolean, true, m_tmp);
                    stk.push({ OPE_PUSH_CONST, id });
                    break;
                case VAR_INTEAGER:
                    id = m_vm.AllocateVar(Integer, true, m_tmp);
                    stk.push({OPE_PUSH_CONST, id});
                    break;
                case VAR_FLOAT:
                    id = m_vm.AllocateVar(Float, true, m_tmp);
                    stk.push({OPE_PUSH_CONST, id});
                    break;
                case VAR_STRING:
                    id = m_vm.AllocateVar(String, true, m_tmp);
                    stk.push({OPE_PUSH_CONST, id});
                    break;
                default:
                    stk.push({OPE_PUSH, (uint)m_ti.info});
                    break;
                }
            }
            else {
                stk.push({m_ti.info, 0});
            }
            NXT
        }
        
    }
    exit(0);
}

RuaControlFLow RuaRuntime::parseControlFlow() {
    //printf("%s %d %s\n", m_parser.GetPosition().c_str(), m_ti.info, m_ti.text.c_str());
    RuaControlFLow cf = RuaControlFLow(SEQUENTIAL);
    if (m_ti.info == '{') {
        cf.type = SEQUENTIAL; NXT
        while (m_ti.info != '}') {
            cf.cfs.push_back(parseControlFlow());
        }
        CHK('}') 
        NXT
        return cf;
    }
    if (m_ti.type == TOK_CF) {
        switch (m_ti.info)
        {
        case FOR:
            cf.type = FOR;
            GETCHK('(') NXT
            cf.cmds.push_back(parseExpression(0));
            cf.cmds.push_back(parseExpression(0));
            cf.cmds.push_back(parseExpression(1));
            CHK(')') NXT
            cf.cfs.push_back(parseControlFlow());
            return cf;
        case WHILE:
            cf.type = WHILE;
            GETCHK('(') 
            NXT
            cf.cmds.push_back(parseExpression(1));
            CHK(')') NXT
            cf.cfs.push_back(parseControlFlow());
            return cf;
        case IF:
            cf.type = IF;
            GETCHK('(') NXT
            cf.cmds.push_back(parseExpression(1));
            CHK(')') NXT
            cf.cfs.push_back(parseControlFlow());
            while (m_ti.info == ELIF) {
                GETCHK('(') NXT
                cf.cmds.push_back(parseExpression(1));
                CHK(')') NXT
                cf.cfs.push_back(parseControlFlow());
            }
            if (m_ti.info == ELSE) {
                NXT
                cf.cfs.push_back(parseControlFlow());
            }
            return cf;
        }
    }
    else {
        cf.cmds.push_back(parseExpression(0));
        return cf;
    }
}

void RuaRuntime::popStack(RuaSentence& rs, std::stack<RuaCommand>& stk) {
    while (!stk.empty() && stk.top().cmd != '(' &&
           m_opeMap[stk.top().cmd].prio <= m_opeMap[m_ti.info].prio) {
        rs.push_back(stk.top());
        stk.pop();
    }
}

int RuaRuntime::runControlFlow(RuaControlFLow* cf) {
    int R;
    uint rid;
    switch (cf->type) {
    case SEQUENTIAL:
        for (auto i : cf->cfs) {
            CHKRET(runControlFlow(&i))
        }
        for (auto i : cf->cmds) {
            CHKRET(runSentence(&i, false))
        }
        return 0;
    case WHILE:
        while (true) {
            CHKRET(runSentence(&cf->cmds[0], true))
            rid = FindRealVar(TOPENV.varstk.top(), false);
            if (!m_vm.GetVar(rid)->data.b) break;
            POPTOP
            R = runControlFlow(&cf->cfs[0]);
            if (R == BREAK) break;
            if (R == RETURN) return RETURN;
        }
        return 0;
    case FOR:
        CHKRET(runSentence(&cf->cmds[0], false))
        while (true) {
            CHKRET(runSentence(&cf->cmds[1], true))
            rid = FindRealVar(TOPENV.varstk.top(), false);
            if (!m_vm.GetVar(rid)->data.b) {
                POPTOP
                break;
            }
            POPTOP
            R = runControlFlow(&cf->cfs[0]);
            if (R == BREAK) break;
            if (R == RETURN) return RETURN;
            CHKRET(runSentence(&cf->cmds[2], false))
        }
        return 0;
    case IF:
        for (int i = 0; i < cf->cmds.size(); i++) {
            CHKRET(runSentence(&cf->cmds[i], true))
            rid = FindRealVar(TOPENV.varstk.top(), false);
            if (m_vm.GetVar(rid)->data.b) {
                POPTOP
                CHKRET(runControlFlow(&cf->cfs[i]));
                return 0;
            }
            else { POPTOP }
        }
        if (cf->cfs.size() > cf->cmds.size())
            CHKRET(runControlFlow(&cf->cfs[cf->cmds.size()]))
        return 0;
    }
    return ERROR;
}

int RuaRuntime::runSentence(RuaSentence* sent, bool keep_top) {
    int R, tt;
    for (auto i = sent->begin(); i != sent->end(); ++i) {
        if (i->cmd == BREAK || i->cmd == CONTINUE) return i->cmd;
        else if (i->cmd == RETURN) {
            if (!TOPENV.varstk.empty()) {
                uLL id = TOPENV.varstk.top();
                if (HIDWORD(id) == 1)
                    m_vm.RefVar(LODWORD(id = FindRealVar(id, false)));
                TOPENV.varstk.top() = LODWORD(id);
            }
            return RETURN;
        }
        else if (i->cmd == ']' || i->cmd == ',') return i - sent->begin();
        else if (i->cmd == '[') {
            uint id = m_vm.AllocateVar(List, false); uLL tid;
            ruaList* lst = m_vm.GetVar(id)->data.l;
            i++;
            while (i->cmd != ']') {
                RuaSentence subrs(i, sent->end());
                i += runSentence(&subrs, true);
                tid = TOPENV.varstk.top();
                if (HIDWORD(tid) == 1) {
                    tid = FindRealVar(tid, false);
                    m_vm.RefVar(tid);
                }
                m_global_env->varmap.insert(std::make_pair(tt = nxtTempTokId(), tid));
                lst->push_back(COMBINE(1, tt));
                TOPENV.varstk.pop();
                if (i->cmd == ',') i++;
            }
            TOPENV.varstk.push(id);
        }
        else {
            CHKRET(runCommand(*i))
        }
    }
    if (!keep_top) while (!TOPENV.varstk.empty()) { POPTOP };
    return 0;
}

int RuaRuntime::runCommand(RuaCommand i) {
    if (i.cmd == OPE_PUSH) {
        TOPENV.varstk.push(COMBINE(1, i.para));
    }
    else if (i.cmd == OPE_PUSH_CONST) {
        TOPENV.varstk.push(m_vm.CopyVar(i.para));
    }
    else {
        int npara = m_opeMap[i.cmd].npara, R = npara;
        uLL* paras = new uLL[npara];
        uint *rids = new uint[npara];
        while (R--) {
            if (TOPENV.varstk.empty()) {
                EasyLog::Write("Runtime (error): Unexpected empty stack.");
                return ERROR;
            }
            paras[R] = TOPENV.varstk.top();
            rids[R] = LODWORD(paras[R]);
            if (HIDWORD(paras[R]) == 1) 
                rids[R] = FindRealVar(rids[R]);
            TOPENV.varstk.pop();
        }
        if (i.cmd == '=') {
            if ((paras[0] >> 32) != 1) {
                EasyLog::Write("Runtime (error): left = must be variable.");
                return ERROR;
            }
            UnrefVar(rids[0]);
            if (HIDWORD(paras[1]) == 1)
                m_vm.RefVar(rids[1]);
            auto iter = TOPENV.varmap.find(LODWORD(paras[0]));
            if (iter == TOPENV.varmap.end()) {
                iter = m_global_env->varmap.find(LODWORD(paras[0]));
                if (iter == m_global_env->varmap.end())
                    TOPENV.varmap[LODWORD(paras[0])] = LODWORD(rids[1]);
                else m_global_env->varmap[LODWORD(paras[0])] = LODWORD(rids[1]);
            }
            else {
                TOPENV.varmap[LODWORD(paras[0])] = LODWORD(rids[1]);
            }
            TOPENV.varstk.push(paras[0]);
        }
        else if (i.cmd == OPE_INDEX) {
            RuaVariable* pvar = m_vm.GetVar((rids[0])), *pv1 = m_vm.GetVar((rids[1]));
            if (pvar->type == List) {
                if (pv1->type != Integer) {
                    EasyLog::Write("Runtime (error): index of list must be Integer, but found " + std::to_string(pv1->type));
                    return ERROR;
                }
                if (pv1->data.i >= 0) 
                    TOPENV.varstk.push(pvar->data.l->at(pv1->data.i));
                else {
                    int idx = (pv1->data.i % pvar->data.l->size());
                    TOPENV.varstk.push(pvar->data.l->at(idx));
                }
            }
            if (HIDWORD(paras[0]) == 0) UnrefVar(rids[0]);
            if (HIDWORD(paras[1]) == 0) UnrefVar(rids[1]);
        }
        else if (i.cmd == OPE_DEL) {
            if (HIDWORD(paras[0]) != 1) {
                EasyLog::Write("Runtime (error): del must be variable.");
                return ERROR;
            } 
            if (rids[0]) {
                m_vm.DeleteVar(rids[0]);
                auto iter = TOPENV.varmap.find(LODWORD(paras[0]));
                if (iter != TOPENV.varmap.end()) 
                    TOPENV.varmap.erase(iter);
                else {
                    iter = m_global_env->varmap.find(LODWORD(paras[0]));
                    m_global_env->varmap.erase(iter);
                }
            }
            else EasyLog::Write("Runtime (warning): del a variable not exist.");
        }
        else if (i.cmd == OPE_CALL) {
            RuaVariable* pvar = m_vm.GetVar(LODWORD(rids[0]));
            if (pvar == nullptr) {
                EasyLog::Write("Runtime (error): called function not exist.");
                exit(-1);
            }
            if (pvar->type == Function) {
                RuaFuncInfo* rfi = pvar->data.f;
                uint ret;
                if (rfi->func_type == FUNC_INBUILT) {
                    ret = ((_ruaFunc*)rfi->data)(this, rids[1]);
                    if (ret > 0) TOPENV.varstk.push(ret);
                    UnrefVar(rids[1]);
                }
                else {
                    ruaList* prs = m_vm.GetVar(rids[1])->data.l;
                    if (rfi->paras.size() != prs->size()) {
                        EasyLog::Write("Runtime (error): parameter not match.");
                        return ERROR;
                    }
                    RuaEnv nxtEnv;
                    if (HIDWORD(paras[0]) == 1 && LODWORD(paras[0]) < BEGIN_OF_TEMP_VAR)
                        nxtEnv.name = m_parser.GetTokenInfo(paras[0])->text;
                    else nxtEnv.name = "__anony_func__:" + std::to_string(rids[0]);
                    for (int it = 0; it < prs->size(); it++) {
                        if (HIDWORD(prs->at(it)) == 1)
                            m_vm.RefVar(nxtEnv.varmap[rfi->paras[it]] = FindRealVar(prs->at(it), false));
                        else
                            nxtEnv.varmap[rfi->paras[it]] = prs->at(it);
                    }
                    m_env_stk.push(nxtEnv);
                    EasyLog::environ_name = &TOPENV.name;
                    R = runControlFlow((RuaControlFLow*)rfi->data);
                    if (R != RETURN && R != 0) return R;
                    uLL result = TOPENV.varstk.empty() ? 0 : TOPENV.varstk.top();
                    for (auto iter : TOPENV.varmap) UnrefVar(iter.second);
                    m_env_stk.pop();
                    EasyLog::environ_name = &TOPENV.name;
                    if (result) TOPENV.varstk.push(result);
                    m_vm.UnrefVar(rids[1]);
                    if (HIDWORD(paras[0]) == 0) UnrefVar(rids[0]);
                }
            }
        }
        else {
            uint ret = g_valueCalculate(&m_vm, rids, i);
            if (ret) {
                TOPENV.varstk.push(ret);
                for (int i = 0; i < npara; i++) if (HIDWORD(paras[i]) == 0) {
                    UnrefVar(rids[i]);
                }
            }
            else {
                //TODO
            }
        }
        delete[] paras; delete[] rids;
    }
    return 0;
}

int RuaRuntime::nxtTempTokId()
{
    auto result = m_global_env->varmap.find(m_nxt_temp_tok_id);
    while (result != m_global_env->varmap.end() && m_nxt_temp_tok_id >= BEGIN_OF_TEMP_VAR) {
        if (m_nxt_temp_tok_id < BEGIN_OF_TEMP_VAR) m_nxt_temp_tok_id = BEGIN_OF_TEMP_VAR - 1;
        result = m_global_env->varmap.find(++m_nxt_temp_tok_id);
    }
    return m_nxt_temp_tok_id++;
}

void RuaRuntime::UnrefVar(uLL tok) {
    uint id = FindRealVar(tok, false);
    if (!id) return;
    RuaVariable* pvar = m_vm.GetVar(id);
    if (!pvar) return;
    if (pvar->type == List && pvar->refCnt == 1) {
        for (auto i : *pvar->data.l) {
            UnrefVar(i);
            m_global_env->varmap.erase(LODWORD(i));
        }
    }
    m_vm.UnrefVar(id);
}

RuaControlFLow*& RuaRuntime::GetGlobalControlFlow()
{
    return m_global_cf;
}

RuaEnv* RuaRuntime::GetGlobalEnvironment()
{
    return m_global_env;
}

uint RuaRuntime::FindRealVar(uLL tokid, bool tl) {
    if (HIDWORD(tokid) == 0 && !tl) return LODWORD(tokid);
    auto iter = TOPENV.varmap.find(LODWORD(tokid));
    if (iter != TOPENV.varmap.end()) return iter->second;
    iter = m_global_env->varmap.find(LODWORD(tokid));
    if (iter != m_global_env->varmap.end()) return iter->second;
    return 0;
}

RuaVarManager* RuaRuntime::GetVarManager()
{
    return &m_vm;
}

RuaRuntime::RuaRuntime() {
    for (int i = 0; i < g_n_operator_info; i++)
        m_opeMap[g_operator_info[i].type] = g_operator_info[i];
    m_env_stk.push(RuaEnv());
    m_global_env = &m_env_stk.top();
    m_global_env->name = "__global__";
    EasyLog::environ_name = &TOPENV.name;

    m_nxt_temp_tok_id = BEGIN_OF_TEMP_VAR;
}

void RuaRuntime::SetConstant(std::string text, RuaVariable value)
{
    int tokID = m_parser.NewToken(TOK_VAR, text, "New Variable");
    uint rid = m_vm.AllocateVar(value.type, true, value.data);
    m_global_env->varmap[tokID] = rid;
}

int RuaRuntime::Preprocess(std::string text) {
    m_parser.PutText(text);
    m_global_cf = new RuaControlFLow(SEQUENTIAL); NXT
    do {
        m_global_cf->cfs.push_back(parseControlFlow());
    } while (m_ti.type != TOK_END);
    //m_global_cf->Print();
    return 0;
}

void RuaRuntime::Run() {
    runControlFlow(m_global_cf);
    TokenInfo* tok_main = m_parser.GetTokenInfo("main");
    if (tok_main == nullptr) {
        EasyLog::Write("Runtime (error): No main found.");
        return;
    }
    auto iter = m_global_env->varmap.find(tok_main->info);
    if (m_vm.GetVar(iter->second)->type != Function) {
        EasyLog::Write("Runtime (error): Main is not a function.");
        return;
    }
    uint tid = m_vm.AllocateVar(List, false);
    std::stack<uLL>& stk = m_env_stk.top().varstk;
    stk.push(COMBINE(1, tok_main->info)); stk.push(tid);
    runCommand({OPE_CALL, (uint)0});
    printf("\n-------------------------\nProgram exited with ");
    if (stk.empty()) printf("no exit code.");
    else {
        printf("return variable : %s", m_vm.to_string(FindRealVar(stk.top(), false)).c_str());
        UnrefVar(stk.top());
    }
}

void RuaRuntime::Execute(std::string program)
{
    m_parser.PutText(program);
    m_global_cf = new RuaControlFLow(SEQUENTIAL); NXT
    do {
        m_global_cf->cfs.push_back(parseControlFlow());
    } while (m_ti.type != TOK_END);
    runControlFlow(m_global_cf);
    delete m_global_cf;
}

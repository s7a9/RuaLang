#ifndef H_RUNTIME
#define H_RUNTIME

#include "Variables.h"
#include "ControlFlow.h"
#include "Parser.h"

struct RuaEnv {
    std::string name;
    std::map<int, uint> varmap;
    std::stack<uLL> varstk;
};

class RuaRuntime {
private:
    uint m_nxt_temp_tok_id;

    RuaVarManager m_vm;

    RuaEnv *m_global_env;

    std::stack<ruaCLass*> m_this_stk;

    std::stack<RuaEnv> m_env_stk;

    RuaParser m_parser;

    RuaData m_tmp;

    TokenInfo m_ti;

    RuaControlFLow* m_global_cf;

    std::map<uint, RuaOpeInfo> m_opeMap;

    std::deque<uint> m_tmpTokCycQueue;

    RuaSentence parseExpression(int prio);

    RuaControlFLow parseControlFlow();

    inline void popStack(RuaSentence& rs, std::stack<RuaCommand>& stk);

    int runControlFlow(RuaControlFLow* cf);

    int runSentence(RuaSentence* sent, bool keep_top);

    int runCommand(RuaCommand cmd);

    void UnrefVar(uLL tok);

    int giveValue(uLL src, uLL dst);

public:

    RuaControlFLow*& GetGlobalControlFlow();

    RuaEnv* GetGlobalEnvironment();

    uint nxtTempTokId();

    uint FindRealVar(uLL tokid, bool translated = true);

    RuaVarManager* GetVarManager();

    typedef uint _ruaFunc(RuaRuntime*, uint);

    RuaRuntime();

    void SetConstant(std::string text, RuaVariable value);

    int Preprocess(std::string text);

    void Run();

    void Execute(std::string program);
};

#endif //H_RUNTIME
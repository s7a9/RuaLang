#ifndef H_VARIABLES
#define H_VARIABLES

#include <string>
#include <vector>
#include <map>
#include <set>
#include "stdafx.h"
#include "ControlFlow.h"

#define VAR_FUNC            2048
#define VAR_CLASS           2049
#define VAR_INTEAGER        2050
#define VAR_FLOAT           2051
#define VAR_STRING          2052
#define VAR_TRUE            2053
#define VAR_FALSE           2054
#define VAR_NEW             10000

enum RuaVarType {
    Boolean, Integer, Float, String, List, Class, Function
};

typedef bool                                    ruaBool;
typedef int                                     ruaInt;
typedef double                                  ruaFloat;
typedef std::string                             ruaString;
typedef std::vector<uLL>                        ruaList;
typedef std::map<uint, uint>                    ruaCLass;

#define FUNC_GLOBAL         0
#define FUNC_INBUILT        1
#define FUNC_MEMBER         2

struct RuaFuncInfo {
    int func_type = 0;
    void* data = nullptr;
    std::vector<uint> paras;
};

union RuaData {
    ruaBool         b;
    ruaInt          i;
    ruaFloat        d;
    ruaString*      s;
    ruaList*        l;
    ruaCLass*       c;
    RuaFuncInfo*    f;
};

struct RuaVariable {
    RuaVarType type;
    RuaData data;
    int refCnt;

    RuaVariable(RuaVarType t, RuaData d, int r);
};

class RuaVarManager {
private:
    std::map<uint, RuaVariable*> m_vars;

    uint m_maxIdx = 1;

public:
    uint CopyVar(uint id);

    uint AllocateVar(RuaVarType type, bool isConstant);

    uint AllocateVar(RuaVarType type, bool isConstant, RuaData data);

    RuaVariable* GetVar(uint idx);

    void UnrefVar(uint idx);

    void DeleteVar(uint idx);

    void RefVar(uint idx);

    std::string to_string(uint idx, bool full = true);
};

#endif //H_VARIABLES
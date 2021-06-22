#ifndef H_CONTROLFLOW
#define H_CONTROLFLOW

#include "stdafx.h"
#include <stack>

#define SEQUENTIAL  4096
#define WHILE       4097
#define FOR         4098
#define IF          4099
#define ELIF        4100
#define ELSE        4101
#define LEAF        4102
#define RETURN      4103
#define BREAK       4104
#define CONTINUE    4015
#define ERROR       (-0x7f7f7f)

struct RuaCommand {
    int cmd;
    uint para;
};

typedef std::vector<RuaCommand> RuaSentence;

struct RuaControlFLow {
    int type;

    std::vector<RuaSentence> cmds;

    std::vector<RuaControlFLow> cfs;

    RuaControlFLow(int t): type(t) {}

    void Print(RuaSentence st) {
        for (auto cmd : st) printf("%d, %d; ", cmd.cmd, cmd.para);
    }

    void Print() {
        switch (type)
        {
        case FOR:
            printf("FOR ( ");
            Print(cmds[0]);
            printf(" || ");
            Print(cmds[1]);
            printf(" || ");
            Print(cmds[2]);
            printf(" ) \n { ");
            cfs[0].Print();
            printf(" } \n");
            break;
        case SEQUENTIAL:
            for(auto st : cfs) {
                st.Print();
                putchar('\n');
            }
            for(auto st : cmds) {
                Print(st);
                putchar('\n');
            }
            break;
        case IF:
            printf("IF ( ");
            Print(cmds[0]);
            printf(" ) \n { ");
            cfs[0].Print();
            for (int i = 1; i < cmds.size(); i++) {
                printf(" } \n ELSEIF ( ");
                Print(cmds[1]);
                printf(" ) \n { ");
                cfs[i].Print();
            }
            if (cfs.size() > cmds.size()) {
                printf(" } \n ELSE { ");
                cfs[cfs.size() - 1].Print();
            }
            printf(" } \n");
            break;
        case WHILE:
            printf("WHILE ( ");
            Print(cmds[0]);
            printf(" ) \n { ");
            cfs[0].Print();
            printf(" }\n");
        default:
            break;
        }
    }
};

#endif //H_CONTROLFLOW
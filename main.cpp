#include "Runtime.h"
#include "InternalFunctions.h"
using namespace std;

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

RuaRuntime rrt;

void runFile(char* path) {
    ifstream ifs;
    ifs.open(path);
    stringstream buffer;
    buffer << ifs.rdbuf();
    std::string contents(buffer.str());
    ifs.close();
    rrt.Preprocess(contents);
    rrt.Run();
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    for (int i = 0; i < RuaInternalFuncCnt; i++) {
        RuaData rd;
        rd.f = new RuaFuncInfo;
        memcpy_s(rd.f, sizeof(RuaFuncInfo), RuaInternalFuncInfoList + i, sizeof(RuaFuncInfo));
        rrt.SetConstant(RuaInternalFunctionNames[i], RuaVariable(Function, rd, 0));
    }
    if (argc >= 2) {
        runFile(argv[1]);
    }
    else {
        string input, program;
        printf("RuaLang alpha 1.2\n");
        while (true)
        {
            printf("\n>>> ");
            getline(cin, input);
            if (input.empty()) continue;
            program += input;
            if (input[input.size() - 1] == '#') {
                program = program.substr(0, program.size() - 1);
                do
                {
                    printf("+>> ");
                    getline(cin, input);
                    program += input;
                } while (input[input.size() - 1] != '#');
                //program = program.substr(0, program.size() - 1);
            }
            rrt.Execute(program);
            program = "";
        }
    }
}
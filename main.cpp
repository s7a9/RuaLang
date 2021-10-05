#include "Runtime.h"
#include "InternalFunctions.h"
using namespace std;

RuaVarManager vm;

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <cmath>
#include <ctime>

RuaRuntime rrt;

void runFile(char* path) {
    ifstream ifs;
    ifs.open(path);
    stringstream buffer;
    buffer << ifs.rdbuf();
    std::string contents(buffer.str());
    cout << "Loading from file: " << path << endl;
    //cout << "------ Program Text ------" << endl << contents << endl << endl;
    ifs.close();

    clock_t strt_time = clock(), end_time;
    //puts("\n----- Preprocess -------");
    rrt.Preprocess(contents);
    end_time = clock();
    cout << "Preprocess finished in " << ((double)end_time - strt_time) / CLOCKS_PER_SEC << " sec." << endl << endl;
    //puts("\n\n------ Run ------");
    rrt.Run();
    end_time = clock();
    cout << "\nFinished in " << ((double)end_time - strt_time) / CLOCKS_PER_SEC;
    puts(" sec. Press any key to exit.");
    _getch();
}

int main(int argc, char* argv[]) {
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
        printf("RuaLang VER 1.0\n");
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
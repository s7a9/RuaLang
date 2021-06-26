#include "Runtime.h"
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

uint _rua_print(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    for (auto iter = rl->begin(); iter != rl->end(); ++iter) {
        uint rid = rt->FindRealVar(*iter, false);
        RuaVariable* pvar = vm->GetVar(rid);
        switch (pvar->type)
        {
        case Integer:
            printf("%d", pvar->data.i); break;
        case Float:
            printf("%lf", pvar->data.d); break;
        case String:
            printf("%s", pvar->data.s->c_str()); break;
        case Boolean:
            printf(pvar->data.b ? "true" : "false"); break;
        default:
            printf(vm->to_string(rid).c_str());
            break;
        }
    }
    return 0;
}

uint _rua_input(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    for (auto iter = rl->begin(); iter != rl->end(); ++iter) {
        uint rid = rt->FindRealVar(*iter, false);
        RuaVariable* pvar = vm->GetVar(rid);
        switch (pvar->type)
        {
        case Integer:
            printf("%d", pvar->data.i); break;
        case Float:
            printf("%lf", pvar->data.d); break;
        case String:
            printf("%s", pvar->data.s->c_str()); break;
        case Boolean:
            printf(pvar->data.b ? "true" : "false"); break;
        default:
            printf(vm->to_string(rid).c_str());
            break;
        }
    }
    string* ret_str = new string;
    cin >> *ret_str;
    RuaData rd;
    rd.s = ret_str;
    return vm->AllocateVar(String, false, rd);
}

uint _rua_int(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function int (error): # of para is not 1.");
        exit(-1);
    }
    auto pvar = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    RuaData rd;
    switch (pvar->type)
    {
    case Boolean: case Integer:
        rd.i = pvar->data.i; break;
    case Float:
        rd.i = (ruaInt)floor(pvar->data.d); break;
    case String:
        rd.i = 0;
        for (auto i : *pvar->data.s) {
            if (!isdigit(i)) {
                EasyLog::Write("External function int (error): parsing a string not number.");
                exit(-1);
            }
            rd.i = rd.i * 10 + i - '0';
        }
        break;
    default:
        EasyLog::Write("External function int (error): parsing other variables.");
        exit(-1);
    }
    return vm->AllocateVar(Integer, false, rd);
}

uint _rua_float(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function float (error): # of para is not 1.");
        exit(-1);
    }
    auto pvar = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    RuaData rd;
    int tmp = 0, i = 0;
    switch (pvar->type)
    {
    case Boolean: case Integer:
        rd.d = pvar->data.i; break;
    case Float:
        rd.d = pvar->data.d; break;
    case String:
        rd.d = .0;
        while (i < pvar->data.s->size() && isdigit(pvar->data.s->at(i)))
            rd.d = rd.d * 10 + pvar->data.s->at(i) - '0', i++;
        if (i < pvar->data.s->size() && pvar->data.s->at(i) == '.')
            while (i < pvar->data.s->size())
                rd.d = rd.d * 10 + pvar->data.s->at(i) - '0', i++, tmp++;
        rd.d /= pow(10, tmp);
        break;
    default:
        EasyLog::Write("External function int (error): parsing other variables.");
        exit(-1);
    }
    return vm->AllocateVar(Float, false, rd);
}

uint _rua_str(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function str (error): # of para is not 1.");
        exit(-1);
    }
    RuaData rd;
    rd.s = new string(vm->to_string(rt->FindRealVar(rl->at(0), false), false));
    return vm->AllocateVar(String, false, rd);
}

uint _rua_len(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    RuaData rd;
    if (rl->size() != 1) {
        EasyLog::Write("External function len (error): # of para is not 1.");
        exit(-1);
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var->type == List) {
        rd.i = var->data.l->size();
    }
    else {
        EasyLog::Write("External function len (error): parameter unsupported.");
        exit(-1);
    }
    return vm->AllocateVar(Integer, false, rd);
}


void runFile(char* path) {
    ifstream ifs;
    ifs.open(path);
    stringstream buffer;
    buffer << ifs.rdbuf();
    std::string contents(buffer.str());
    cout << "Loading from text: " << path << endl;
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
    string exe_path(argv[0]);
    while (exe_path[exe_path.size() - 1] != '\\')
        exe_path.pop_back();
    exe_path += "log.txt";
    EasyLog::log_path = (char*)exe_path.c_str();

    RuaFuncInfo rfi[] = { 
        {FUNC_INBUILT, _rua_print,  {} },
        {FUNC_INBUILT, _rua_input,  {} },
        {FUNC_INBUILT, _rua_int,    {} },
        {FUNC_INBUILT, _rua_float,  {} },
        {FUNC_INBUILT, _rua_str,    {} },
        {FUNC_INBUILT, _rua_len,    {}} };
    const int rfi_n = sizeof(rfi) / sizeof(RuaFuncInfo);
    string func_name[] = { "print", "input" , "int", "float", "str", "len" };
    //RuaData rd; rd.f = &rfi1;
    //rrt.SetConstant("print", RuaVariable(Function, rd, 0));
    for (int i = 0; i < rfi_n; i++) {
        RuaData rd;
        rd.f = rfi + i;
        rrt.SetConstant(func_name[i], RuaVariable(Function, rd, 0));
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
            if (input == "exit") break;
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
                program = program.substr(0, program.size() - 1);
            }
            rrt.Execute(program);
            program = "";
        }
    }
}
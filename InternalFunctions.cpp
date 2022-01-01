#include "InternalFunctions.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

void _rua_print_recursion(uLL rid, RuaVarManager* vm, RuaRuntime* rt) {
    auto pvar = vm->GetVar(rid = rt->FindRealVar(rid, false));
    if (pvar == nullptr) {
        printf("NULL");
        return;
    }
    switch (pvar->type)
    {
    case Integer:
        printf("%lld", pvar->data.i); break;
    case Float:
        printf("%lf", pvar->data.d); break;
    case String:
        printf("%s", pvar->data.s->c_str()); break;
    case Boolean:
        printf(pvar->data.b ? "true" : "false"); break;
    case List:
        printf("[ ");
        for (auto i : *pvar->data.l) {
            _rua_print_recursion(i, vm, rt);
            putchar(' ');
        }
        putchar(']');
        break;
    default:
        printf(vm->to_string(rid).c_str()); break;
    }
}

uint _rua_print(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    for (auto iter = rl->begin(); iter != rl->end(); ++iter)
        _rua_print_recursion(rt->FindRealVar(*iter, false), vm, rt);
    return 0;
}

uint _rua_input(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    for (auto iter = rl->begin(); iter != rl->end(); ++iter) {
        uint rid = rt->FindRealVar(*iter, false);
        RuaVariable* pvar = vm->GetVar(rid);
        if (pvar == nullptr) {
            printf("NULL");
            continue;
        }
        switch (pvar->type)
        {
        case Integer:
            printf("%lld", pvar->data.i); break;
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
    ruaString* ret_str = new ruaString;
    std::cin >> *ret_str;
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
    if (pvar == nullptr) {
        EasyLog::Write("External function int (error): parament is null.");
        exit(-1);
    }
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
    if (pvar == nullptr) {
        EasyLog::Write("External function float (error): parament is null.");
        exit(-1);
    }
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
    rd.s = new ruaString(vm->to_string(rt->FindRealVar(rl->at(0), false), false));
    return vm->AllocateVar(String, false, rd);
}

uint _rua_str_to_list(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    RuaData rd, itv;
    rd.l = new ruaList;
    for (uLL item : *rl) {
        auto pvar = vm->GetVar(rt->FindRealVar(item, false));
        if (pvar == nullptr) {
            EasyLog::Write("External function str_to_list (error): parament is null.");
            exit(-1);
        }
        if (pvar->type != String) {
            EasyLog::Write("External function str_to_list (error): parament is not string.");
            exit(-1);
        }
        for (char& ch : *pvar->data.s) {
            itv.i = ch;
            int tmpid = rt->nxtTempTokId();
            uint rid = vm->AllocateVar(Integer, false, itv);
            rt->GetGlobalEnvironment()->varmap[tmpid] = rid;
            rd.l->push_back(COMBINE(1, tmpid));
        }
    }
    return vm->AllocateVar(List, false, rd);
}

uint _rua_list_to_str(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    RuaData rd;
    rd.s = new ruaString;
    for (uLL item : *rl) {
        auto pvar = vm->GetVar(rt->FindRealVar(item, false));
        if (pvar == nullptr) {
            EasyLog::Write("External function list_to_str (error): parament is null.");
            exit(-1);
        }
        if (pvar->type != List) {
            EasyLog::Write("External function list_to_str (error): parament is not list.");
            exit(-1);
        }
        for (uLL& intch : *pvar->data.l) {
            auto pitem = vm->GetVar(rt->FindRealVar(intch, false));
            if (pitem == nullptr) {
                EasyLog::Write("External function list_to_str (error): chararcter is null.");
                exit(-1);
            }
            if (pitem->type != Integer) {
                EasyLog::Write("External function list_to_str (error): chararcter is not int.");
                exit(-1);
            }
            rd.s->push_back(pitem->data.i);
        }
    }
    return vm->AllocateVar(String, false, rd);
}

#include <iostream>

uint _rua_breakpoint(RuaRuntime* rt, uint list_id)
{
    std::string program, input;
    while (input != "dbexit")
    {
        printf("\n(debug) >>> ");
        std::getline(std::cin, input);
        if (input.empty()) continue;
        program += input;
        if (input[input.size() - 1] == '#') {
            program = program.substr(0, program.size() - 1);
            do
            {
                printf("(debug) +>> ");
                std::getline(std::cin, input);
                program += input;
            } while (input[input.size() - 1] != '#');
            //program = program.substr(0, program.size() - 1);
        }
        rt->Execute(program);
        program = "";
    }
    return 0;
}

uint _rua_id(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    RuaData rd;
    rd.i = rt->FindRealVar(rl->at(0), false);
    return vm->AllocateVar(Integer, false, rd);
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
    if (var == nullptr) {
        EasyLog::Write("External function len (error): parament is null.");
        exit(-1);
    }
    if (var->type == List) {
        rd.i = var->data.l->size();
    }
    else if (var->type == String) {
        rd.i = var->data.s->size();
    }
    else {
        EasyLog::Write("External function len (error): parameter unsupported.");
        exit(-1);
    }
    return vm->AllocateVar(Integer, false, rd);
}

uint _rua_read_file(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function read_file (error): # of para is not 1.");
        exit(-1);
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr) {
        EasyLog::Write("External function read_file (error): path is null.");
        exit(-1);
    }
    if (var->type != String) {
        EasyLog::Write("External function read_file (error): path is not string.");
        exit(-1);
    }
    ruaString* ps = var->data.s;
    if (!std::filesystem::exists(std::filesystem::path(*ps))) {
        EasyLog::Write("External function read_file (error): file \"" + *ps + "\" not exist.");
        exit(-1);
    }
    std::ifstream hfile(ps->c_str());
    std::stringstream buffer;
    buffer << hfile.rdbuf();
    RuaData rd;
    rd.s = new ruaString(buffer.str());
    hfile.close();
    return vm->AllocateVar(String, false, rd);
}

uint _rua_exist(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function exist (error): # of para is not 1.");
        exit(-1);
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr) {
        EasyLog::Write("External function read_file (error): path is null.");
        exit(-1);
    }
    if (var->type != String) {
        EasyLog::Write("External function exist (error): path is not string.");
        exit(-1);
    }
    RuaData rd;
    rd.b = std::filesystem::exists(std::filesystem::path(*var->data.s));
    return vm->AllocateVar(Boolean, false, rd);
}

uint _rua_write_file(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 2) {
        EasyLog::Write("External function write_file (exited): # of para is not 2.");
        return 0;
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false)),
        var_content = vm->GetVar(rt->FindRealVar(rl->at(1), false));
    if (var == nullptr) {
        EasyLog::Write("External function read_file (exited): path is null.");
        return 0;
    }
    if (var_content == nullptr) {
        EasyLog::Write("External function read_file (exited): content is null.");
        return 0;
    }
    if (var->type != String) {
        EasyLog::Write("External function write_file (exited): path is not string.");
        return 0;
    }
    if (var_content->type != String) {
        EasyLog::Write("External function write_file (exited): content is not string.");
        return 0;
    }
    ruaString* ps = var->data.s;
    std::ofstream hfile(ps->c_str());
    hfile << var_content->data.s->c_str();
    hfile.close();
    return 0;
}

uint _rua_execute(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function execute (exited): # of para is not 1.");
        return 0;
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr) {
        EasyLog::Write("External function read_file (exited): command is null.");
        return 0;
    }
    if (var->type != String) {
        EasyLog::Write("External function execute (exited): parament is not string.");
        return 0;
    }
    RuaControlFLow* lastCF = rt->GetGlobalControlFlow();
    rt->Execute(*var->data.s);
    rt->GetGlobalControlFlow() = lastCF;
    return 0;
}

uint _rua_is_null(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function execute (error): # of para is not 1.");
        exit(-1);
    }
    RuaData rd;
    rd.b = vm->GetVar(rt->FindRealVar(rl->at(0), false)) == nullptr;
    return vm->AllocateVar(Boolean, false, rd);
}

uint _rua_append(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr) {
        EasyLog::Write("External function append (exited): parament is null.");
        return 0;
    }
    if (var->type != List) {
        EasyLog::Write("External function append (exited): parameter 1 is not list.");
        return 0;
    }
    for (int i = 1; i < rl->size(); i++) {
        uLL rid = rt->FindRealVar(rl->at(i), false);
        int tmp;
        if (rid == 0) 
            var->data.l->push_back(0);
        else{
            tmp = rt->nxtTempTokId();
            rt->GetGlobalEnvironment()->varmap[tmp] = LODWORD(rid);
            vm->RefVar(rid);
            var->data.l->push_back(COMBINE(1, tmp));
        }
    }
    return 0;
}

uint _rua_typeof(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    RuaData rd;
    if (rl->size() != 1) {
        EasyLog::Write("External function type (error): # of para is not 1.");
        exit(-1);
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr)
        rd.i = -1;
    else
        rd.i = (int)var->type;
    return vm->AllocateVar(Integer, false, rd);
}

uint _rua_exit(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() == 0) {
        printf("Program exited.");
        exit(0);
    }
    printf("Program exited with code ");
    for (auto id : *rl) {
        _rua_print_recursion(id, vm, rt);
    }
    exit(0);
}

uint _rua_system(RuaRuntime* rt, uint list_id) {
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    if (rl->size() != 1) {
        EasyLog::Write("External function system (exited): # of para is not 1.");
        return 0;
    }
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var->type != String) {
        EasyLog::Write("External function system (error): command is not string.");
        exit(-1);
    }
    system(var->data.s->c_str());
    return 0;
}

uint _rua_table(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    return vm->AllocateVar(Class, false);
}

uint _rua_randn(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    RuaData rd;
    rd.i = (ruaInt)rand() << 32 + rand();
    return vm->AllocateVar(Integer, false, rd);
}

uint _rua_remove(RuaRuntime* rt, uint list_id)
{
    auto vm = rt->GetVarManager();
    ruaList* rl = vm->GetVar(list_id)->data.l;
    auto var = vm->GetVar(rt->FindRealVar(rl->at(0), false));
    if (var == nullptr) {
        EasyLog::Write("External function remove (exited): parament is null.");
        return 0;
    }
    if (var->type != List) {
        EasyLog::Write("External function remove (exited): parameter 1 is not list.");
        return 0;
    }
    int n_removed = 0;
    for (int i = 1; i < rl->size(); i++) {
        auto pvar = vm->GetVar(rt->FindRealVar(rl->at(i), false));
        if (pvar->type != Integer) {
            EasyLog::Write("External function remove (exited): index is not integer.");
            continue;
        }
        int to_rm = (int)pvar->data.i - n_removed;
        if (to_rm < 0 || to_rm >= var->data.l->size()) {
            EasyLog::Write("External function remove (exited): index out of range.");
            continue;
        }
        var->data.l->erase(var->data.l->begin() + to_rm);
        n_removed++;
    }
    return 0;
}

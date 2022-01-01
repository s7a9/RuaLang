#pragma once

#include "Runtime.h"

uint _rua_print(RuaRuntime* rt, uint list_id);

uint _rua_input(RuaRuntime* rt, uint list_id);

uint _rua_int(RuaRuntime* rt, uint list_id);

uint _rua_float(RuaRuntime* rt, uint list_id);

uint _rua_str(RuaRuntime* rt, uint list_id);

uint _rua_str_to_list(RuaRuntime* rt, uint list_id);

uint _rua_list_to_str(RuaRuntime* rt, uint list_id);

uint _rua_len(RuaRuntime* rt, uint list_id);

uint _rua_read_file(RuaRuntime* rt, uint list_id);

uint _rua_exist(RuaRuntime* rt, uint list_id);

uint _rua_write_file(RuaRuntime* rt, uint list_id);

uint _rua_execute(RuaRuntime* rt, uint list_id);

uint _rua_is_null(RuaRuntime* rt, uint list_id);

uint _rua_append(RuaRuntime* rt, uint list_id);

uint _rua_typeof(RuaRuntime* rt, uint list_id);

uint _rua_exit(RuaRuntime* rt, uint list_id);

uint _rua_list_to_str(RuaRuntime* rt, uint list_id);

uint _rua_breakpoint(RuaRuntime* rt, uint list_id);

uint _rua_id(RuaRuntime* rt, uint list_id);

uint _rua_system(RuaRuntime* rt, uint list_id);

uint _rua_table(RuaRuntime* rt, uint list_id);

uint _rua_randn(RuaRuntime* rt, uint list_id);

uint _rua_remove(RuaRuntime* rt, uint list_id);

const RuaFuncInfo RuaInternalFuncInfoList[] = {
    {FUNC_INBUILT, _rua_print,  {} },
    {FUNC_INBUILT, _rua_input,  {} },
    {FUNC_INBUILT, _rua_int,    {} },
    {FUNC_INBUILT, _rua_float,  {} },
    {FUNC_INBUILT, _rua_str,    {} },
    {FUNC_INBUILT, _rua_len,    {} },
    {FUNC_INBUILT, _rua_read_file,  {}},
    {FUNC_INBUILT, _rua_exist,  {} },
    {FUNC_INBUILT, _rua_write_file,  {}},
    {FUNC_INBUILT, _rua_execute,{} },
    {FUNC_INBUILT, _rua_is_null,{} },
    {FUNC_INBUILT, _rua_append, {} },
    {FUNC_INBUILT, _rua_typeof, {} },
    {FUNC_INBUILT, _rua_exit,   {} },
    {FUNC_INBUILT, _rua_str_to_list, {} },
    {FUNC_INBUILT, _rua_list_to_str, {} },
    {FUNC_INBUILT, _rua_breakpoint, {} },
    {FUNC_INBUILT, _rua_id, {}},
    {FUNC_INBUILT, _rua_system, {}},
    {FUNC_INBUILT, _rua_table, {}},
    {FUNC_INBUILT, _rua_randn, {}},
    {FUNC_INBUILT, _rua_remove, {}},
};

const std::string RuaInternalFunctionNames[] = {
     "print", "input" , "int", "float", "str", "len", "read_file", "exist", 
     "write_file", "exec", "is_null", "append", "typeof", "exit", "str_to_list",
     "list_to_str", "breakpoint", "_id_", "system", "table", "randn", "remove",
};

const int RuaInternalFuncCnt = sizeof(RuaInternalFuncInfoList) / sizeof(RuaFuncInfo);
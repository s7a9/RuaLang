#ifndef H_OPERATORS
#define H_OPERATORS

#include <string>
#include "stdafx.h"

#define OPE_PUSH            0
#define OPE_PUSH_CONST      1

#define OPE_NEG             1024
#define OPE_MOVE_LEFT       1025
#define OPE_MOVE_RIGHT      1026
#define OPE_INDEX           1027
#define OPE_EQUAL           1028
#define OPE_AND             1029
#define OPE_LEQ             1030
#define OPE_GEQ             1031
#define OPE_NEQ             1032
#define OPE_OR              1033
#define OPE_DEL             1034
#define OPE_CALL            1035

struct RuaOpeInfo {
    int type, prio, npara;
};

const RuaOpeInfo g_operator_info[] = {
    {'+', 4, 2}, {'-', 4, 2}, {'*', 3, 2}, {'/', 3, 2}, 
    {'\\', 3, 2}, {OPE_MOVE_LEFT, 5, 2}, {OPE_MOVE_RIGHT, 5, 2},
    {'<', 6, 2}, {OPE_EQUAL, 7, 2}, {'>', 6, 2}, {OPE_LEQ, 6, 2},
    {OPE_GEQ, 6, 2}, {OPE_NEQ, 7, 2}, {'(', 1, 0}, {')', 100, 0},
    {'=', 10, 2}, {'&', 2, 1}, {'.', 1, 2}, {';', 100, 0}, {',', 100, 0},
    {OPE_NEQ, 7, 2}, {OPE_INDEX, 99, 2}, {'%', 3, 2}, {OPE_AND, 8, 2},
    {OPE_OR, 9, 2}, {'{', -1, 0}, {'}', 100, 0}, {OPE_PUSH, 0, 1}, 
    {OPE_PUSH_CONST, 0, 1}, {']', 100, 0}, {3, -1, 0}, {BREAK, 100, 0},
    {RETURN, 100, 0}, {OPE_DEL, 99, 1}, {OPE_CALL, 1, 2}
};

const int g_n_operator_info = sizeof(g_operator_info) / sizeof(RuaOpeInfo);

#endif //H_OPERATORS
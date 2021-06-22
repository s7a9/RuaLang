#ifndef H_CALCULATOR
#define H_CALCULATOR

#include "logging.hpp"
#include "Variables.h"
#include "Operators.h"

namespace RUA {

	uint g_rua_add(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		if (v1->type < v2->type) return g_rua_add(vm, v2, v1);
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b + v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.i = v1->data.i + v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		case Float:
			if (v2->type == Float)
				rd.d = v1->data.d + v2->data.d;
			else
				rd.d = v1->data.d + v2->data.i;
			return vm->AllocateVar(Float, false, rd);
		case String:
			if (v2->type == String) {
				rd.s = new std::string(*v1->data.s + *v2->data.s);
				return vm->AllocateVar(String, false, rd);
			}
			else {
				EasyLog::Write("Runtime internal operator + (Error): add string with other variables!");
				exit(-1);
			}
		}
		return 0;
	}

	uint g_rua_minus(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Inteager:
			if (v2->type == Inteager || v2->type == Boolean) {
				rd.i = v1->data.i - v2->data.i;
				return vm->AllocateVar(Inteager, false, rd);
			}
			else if (v2->type == Float) {
				rd.d = v1->data.i - v2->data.d;
				return vm->AllocateVar(Float, false, rd);
			}
			else {
				EasyLog::Write("Runtime internal operator - (Error): int minus right ope!");
				exit(-1);
			}
		case Float:
			if (v2->type == Float)
				rd.d = v1->data.d - v2->data.d;
			else if (v2->type == Inteager || v2->type == Boolean)
				rd.d = v1->data.d - v2->data.i;
			else {
				EasyLog::Write("Runtime internal operator - (Error): int minus right ope!");
				exit(-1);
			}
			return vm->AllocateVar(Float, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator - (Error): minus string!");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_multiply(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		if (v1->type < v2->type) return g_rua_multiply(vm, v2, v1);
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b * v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.i = v1->data.i * v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		case Float:
			if (v2->type == Float)
				rd.d = v1->data.d * v2->data.d;
			else
				rd.d = v1->data.d * v2->data.i;
			return vm->AllocateVar(Float, false, rd);
		case String:
			if (v2->type == Inteager || v2->type == Boolean) {
				rd.s = new std::string();
				for (int i = 0; i < v2->data.i; i++) {
					rd.s->append(*v1->data.s);
				}
				return vm->AllocateVar(String, false, rd);
			}
			else {
				EasyLog::Write("Runtime internal operator * (Error): multiply string!");
				exit(-1);
			}
		}
		return 0;
	}

	uint g_rua_divide(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b / v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			if (v1->data.i % v2->data.i == 0) {
				rd.i = v1->data.i / v2->data.i;
				return vm->AllocateVar(Inteager, false, rd);
			}
			else {
				rd.d = (ruaFloat)v1->data.i / v2->data.i;
				return vm->AllocateVar(Float, false, rd);
			}
		case Float:
			if (v2->type == Float)
				rd.d = v1->data.d / v2->data.d;
			else
				rd.d = v1->data.d / v2->data.i;
			return vm->AllocateVar(Float, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator / (Error): divide string!");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_int_divide(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean: case Inteager:
			rd.i = v1->data.i / v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		default:
			EasyLog::Write("Runtime internal operator \\ (Error): int divide");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_move_left(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b << v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.i = v1->data.i << v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		default:
			EasyLog::Write("Runtime internal operator << (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_move_right(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b >> v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.i = v1->data.i >> v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		default:
			EasyLog::Write("Runtime internal operator >> (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_less(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean: case Inteager:
			if (v2->type == Inteager || v2->type == Boolean) {
				rd.b = v1->data.i < v2->data.i;
			}
			else if (v2->type == Float) {
				rd.b = v1->data.i < v2->data.d;
			}
			else {
				EasyLog::Write("Runtime internal operator < (Error): int minus right ope!");
				exit(-1);
			}
			return vm->AllocateVar(Boolean, false, rd);
		case Float:
			if (v2->type == Float)
				rd.b = v1->data.d < v2->data.d;
			else if (v2->type == Inteager || v2->type == Boolean)
				rd.b = v1->data.d < v2->data.i;
			else {
				EasyLog::Write("Runtime internal operator < (Error): int minus right ope!");
				exit(-1);
			}
			return vm->AllocateVar(Boolean, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator < (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_equal(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		if (v1->type < v2->type) return g_rua_equal(vm, v2, v1);
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b == v2->data.b; break;
		case Inteager:
			rd.b = v1->data.i == v2->data.i; break;
		case Float:
			if (v2->type == Float)
				rd.b = v1->data.d == v2->data.d;
			else
				rd.b = v1->data.d == v2->data.i;
			break;
		case String:
			if (v2->type != String) {
				EasyLog::Write("Runtime internal operator == (Error): compare string with other");
				exit(-1);
			}
			rd.b = *v1->data.s == *v2->data.s; break;
		case List:
			rd.b = *v1->data.s == *v2->data.s; break;
		default:
			rd.i = 0;
		}
		return vm->AllocateVar(Boolean, false, rd);
	}

	uint g_rua_greater(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean: case Inteager:
			if (v2->type == Inteager || v2->type == Boolean) {
				rd.b = v1->data.i > v2->data.i;
			}
			else if (v2->type == Float) {
				rd.b = v1->data.i > v2->data.d;
			}
			else {
				EasyLog::Write("Runtime internal operator > (Error): int minus right ope!");
				exit(-1);
			}
			return vm->AllocateVar(Boolean, false, rd);
		case Float:
			if (v2->type == Float)
				rd.b = v1->data.d > v2->data.d;
			else if (v2->type == Inteager || v2->type == Boolean)
				rd.b = v1->data.d > v2->data.i;
			else {
				EasyLog::Write("Runtime internal operator > (Error): int minus right ope!");
				exit(-1);
			}
			return vm->AllocateVar(Boolean, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator < (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_leq(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		uint ret = g_rua_greater(vm, v1, v2);
		if (!ret) return 0;
		vm->GetVar(ret)->data.b = !vm->GetVar(ret)->data.b;
		return ret;
	}

	uint g_rua_geq(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		uint ret = g_rua_less(vm, v1, v2);
		if (!ret) return 0;
		vm->GetVar(ret)->data.b = !vm->GetVar(ret)->data.b;
		return ret;
	}

	uint g_rua_neq(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		uint ret = g_rua_equal(vm, v1, v2);
		if (!ret) return 0;
		vm->GetVar(ret)->data.b = !vm->GetVar(ret)->data.b;
		return ret;
	}

	uint g_rua_mod(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean: case Inteager:
			rd.i = v1->data.i % v2->data.i;
			return vm->AllocateVar(Inteager, false, rd);
		default:
			EasyLog::Write("Runtime internal operator % (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_and(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		if (v1->type < v2->type) return g_rua_and(vm, v2, v1);
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b && v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.b = v1->data.i && v2->data.i;
			return vm->AllocateVar(Boolean, false, rd);
		case Float:
			if (v2->type == Float)
				rd.b = v1->data.d && v2->data.d;
			else
				rd.b = v1->data.d && v2->data.i;
			return vm->AllocateVar(Boolean, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator && (Error)");
			exit(-1);
		}
		return 0;
	}

	uint g_rua_or(RuaVarManager* vm, RuaVariable* v1, RuaVariable* v2) {
		if (v1->type >= 5) return 0;
		if (v1->type < v2->type) return g_rua_or(vm, v2, v1);
		RuaData rd; uint ret;
		switch (v1->type)
		{
		case Boolean:
			rd.b = v1->data.b || v2->data.b;
			return vm->AllocateVar(Boolean, false, rd);
		case Inteager:
			rd.b = v1->data.i || v2->data.i;
			return vm->AllocateVar(Boolean, false, rd);
		case Float:
			if (v2->type == Float)
				rd.b = v1->data.d || v2->data.d;
			else
				rd.b = v1->data.d || v2->data.i;
			return vm->AllocateVar(Boolean, false, rd);
		case String:
			EasyLog::Write("Runtime internal operator || (Error)");
			exit(-1);
		}
		return 0;
	}
}

uint g_valueCalculate(RuaVarManager* vm, uint* paras, RuaCommand& cmd) {
	switch (cmd.cmd)
	{
	case '+': return RUA::g_rua_add(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '-': return RUA::g_rua_minus(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '*': return RUA::g_rua_multiply(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '/': return RUA::g_rua_divide(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '\\': return RUA::g_rua_int_divide(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_MOVE_LEFT: return RUA::g_rua_move_left(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_MOVE_RIGHT: return RUA::g_rua_add(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '<': return RUA::g_rua_less(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_EQUAL: return RUA::g_rua_equal(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '>': return RUA::g_rua_greater(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_LEQ: return RUA::g_rua_leq(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_GEQ: return RUA::g_rua_geq(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_NEQ: return RUA::g_rua_neq(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case '%': return RUA::g_rua_mod(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_AND: return RUA::g_rua_and(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	case OPE_OR: return RUA::g_rua_or(vm, vm->GetVar(paras[0]), vm->GetVar(paras[1]));
	}
    return 0;
}

#endif //H_CALCULATOR
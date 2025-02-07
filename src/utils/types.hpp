#pragma once

#include <iostream>
#include <vector>
#include <map>

typedef          char   i8;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

typedef std::string                           token;
typedef std::vector<std::string>              vectok;
typedef std::vector<std::vector<std::string>> part;


enum commands
{
    _RGWRT         = 0b00,
    _MOV           = 0b01,
    _OPERATIONS    = 0b10,
    _CONDITIONALS  = 0b11
};

enum operations
{
    _ADD,
    _SUB,
    _AND,
    _OR,
    _XOR,
    _NOT,
    _SHR,
    _SHL,
    _INC,
    _DEC,
};

enum conditionals
{
    _JMP,
    _JEZ,
    _JGZ,
    _JLZ
};

enum registers
{
    _rg0,
    _rg1,
    _rg2,
    _rg3,
    _ma,
    _mem,
    _in,
    _out,
};

std::map<std::string, commands> comms = {
    {"wrt", commands::_RGWRT},
    {"mov", commands::_MOV},
    {"add", commands::_OPERATIONS},
    {"sub", commands::_OPERATIONS},
    {"and", commands::_OPERATIONS},
    {"or",  commands::_OPERATIONS},
    {"xor", commands::_OPERATIONS},
    {"not", commands::_OPERATIONS},
    {"shr", commands::_OPERATIONS},
    {"shl", commands::_OPERATIONS},
    {"inc", commands::_OPERATIONS},
    {"dec", commands::_OPERATIONS},
    {"jmp", commands::_CONDITIONALS},
    {"jez", commands::_CONDITIONALS},
    {"jgz", commands::_CONDITIONALS},
    {"jlz", commands::_CONDITIONALS},
};
std::map<std::string, operations> ops = {
    {"add", operations::_ADD},
    {"sub", operations::_SUB},
    {"and", operations::_AND},
    {"or",  operations::_OR},
    {"xor", operations::_XOR},
    {"not", operations::_NOT},
    {"shr", operations::_SHR},
    {"shl", operations::_SHL},
    {"inc", operations::_INC},
    {"dec", operations::_DEC},
};
std::map<std::string, conditionals> conds = {
    {"jmp", conditionals::_JMP},
    {"jez", conditionals::_JEZ},
    {"jgz", conditionals::_JGZ},
    {"jlz", conditionals::_JLZ},
};

std::map<std::string, registers> regs = {
    {"rg0", registers::_rg0},
    {"rg1", registers::_rg1},
    {"rg2", registers::_rg2},
    {"rg3", registers::_rg3},
    {"ma",  registers::_ma},
    {"mem", registers::_mem},
    {"in",  registers::_in},
    {"out", registers::_out},
};
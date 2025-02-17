#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "utils/types.hpp"

class Process
{
public:
    i8 rg0 = 0;
    i8 rg1 = 0;
    i8 rg2 = 0;
    i8 rg3 = 0;
    u8 ma  = 0;  // mem address
    u8 in  = 0;  // input
    u8 out = 0;  // output
    i8 mem[256] = {};
    
    void set(i8 num, i8 val)
    {
        if (num == registers::_rg0) rg0     = val;
        if (num == registers::_rg1) rg1     = val;
        if (num == registers::_rg2) rg2     = val;
        if (num == registers::_rg3) rg3     = val;
        if (num == registers::_ma)  ma      = val;
        if (num == registers::_mem) mem[ma] = val;
        if (num == registers::_in)  in      = val;
        if (num == registers::_out) out     = val;
    }

    i8 get(i8 num)
    {
        if (num == registers::_rg0) return rg0;
        if (num == registers::_rg1) return rg1;
        if (num == registers::_rg2) return rg2;
        if (num == registers::_rg3) return rg3;
        if (num == registers::_ma)  return ma;
        if (num == registers::_mem) return mem[ma];
        if (num == registers::_in)  return in;
        if (num == registers::_out) return out;
        return 0;
    }
};

struct PPbuffers
{
    u8   prgm_buf[65536]  = {};
    u16  prgm_buf_index   = 0;
    i8   long_buf[65536]  = {};
    u16  long_buf_index   = 0;
};

struct PPinfo
{
    Process *curent_process;
    PPbuffers *buffers;
};
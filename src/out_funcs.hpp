#pragma once

#include "process.hpp"

#define OUTF_ARGS (PPinfo pp_info, std::vector<i8> args)
#define OUTF_INIT Process *proc = pp_info.curent_process;\
                  PPbuffers *bufs = pp_info.buffers;



void out0x05 OUTF_ARGS
{
    OUTF_INIT
    std::cout << args[0];
}

void out0x07 OUTF_ARGS
{
    OUTF_INIT
    std::cout << +args[0];
}

void out0x09 OUTF_ARGS
{
    OUTF_INIT
    bufs->long_buf_index = args[0];
}

void out0x0B OUTF_ARGS
{
    OUTF_INIT
    bufs->long_buf[bufs->long_buf_index] = args[0];
    bufs->long_buf_index++;
}
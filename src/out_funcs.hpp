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
    bufs->long_buf_index = (args[0]<<8) + (args[1]);
}

void out0x0B OUTF_ARGS
{
    OUTF_INIT
    bufs->long_buf[bufs->long_buf_index] = args[0];
    bufs->long_buf_index++;
}

void out0x23 OUTF_ARGS
{
    OUTF_INIT
    pp_info.disk_index = (args[0]<<8)+args[1];
}

void out0x26 OUTF_ARGS
{
    OUTF_INIT
    for (int i = 0; i < args[0]; i++) {
        proc->mem[proc->ma++] = pp_info.disk[pp_info.disk_index++];
    }
}

void out0x27 OUTF_ARGS
{
    OUTF_INIT
    pp_info.disk[pp_info.disk_index] = args[0];
}

void out0x28 OUTF_ARGS
{
    OUTF_INIT
    for (int i = 0; i < args[0]; i++) {
        pp_info.disk[pp_info.disk_index++] = proc->mem[proc->ma++];
    }
}

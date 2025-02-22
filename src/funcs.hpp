#pragma once

#include "process.hpp"
#include "out_funcs.hpp"

void ops_exeting(PPinfo pp_info, i8 op)
{
    Process *proc = pp_info.curent_process;

    switch (op)
    {
        case operations::_ADD:
            proc->rg1 = proc->rg1 + proc->rg2;
        break;
        
        case operations::_SUB:
            proc->rg1 = proc->rg1 - proc->rg2;
        break;
        
        case operations::_AND:
            proc->rg1 = proc->rg1 & proc->rg2;
        break;
        
        case operations::_OR:
            proc->rg1 = proc->rg1 | proc->rg2;
        break;
        
        case operations::_XOR:
            proc->rg1 = proc->rg1 ^ proc->rg2;
        break;
        
        case operations::_NOT:
            proc->rg1 = !proc->rg1;
        break;
        
        case operations::_SHR:
            proc->rg1 = proc->rg1 >> proc->rg2;
        break;
        
        case operations::_SHL:
            proc->rg1 = proc->rg1 << proc->rg2;
        break;

        case operations::_INC:
            proc->ma++;
        break;

        case operations::_DEC:
            proc->ma--;
        break;
    }
}

u16 conds_executing(PPinfo pp_info, i8 op, i8 reg, i8 page, u8 jmpaddr)
{
    Process *proc = pp_info.curent_process;

    switch (op)
    {
        case conditionals::_JMP:
            if (jmpaddr == 0) {
                return proc->get(reg) * (page*256);
            }
            return jmpaddr + (page*256);
        break;

        case conditionals::_JEZ:
            if (proc->get(reg) == 0) {
                return jmpaddr + (page*256);
            }
        break;

        case conditionals::_JGZ:
            if (proc->get(reg) > 0) {
                return jmpaddr + (page*256);
            }
        break;

        case conditionals::_JLZ:
            if (proc->get(reg) < 0) {
                return jmpaddr + (page*256);
            }
        break;
    }
    return pp_info.buffers->prgm_buf_index+1;
}

static u8 out_subfunc_args_num = 0;
static std::vector<i8> out_subfunc_args {};
static void (*out_subfunc) (PPinfo, std::vector<i8>) = nullptr;

void out_analis(PPinfo pp_info)
{
    Process *proc = pp_info.curent_process;
    PPbuffers *buffers = pp_info.buffers;

    if (out_subfunc_args_num > 0) {
        out_subfunc_args.push_back(proc->out);
        out_subfunc_args_num--;

        if (out_subfunc_args_num == 0) {
            if (out_subfunc != nullptr) {
                out_subfunc(pp_info, out_subfunc_args);
                out_subfunc = nullptr;
                out_subfunc_args.clear();
            }
        }
        return;
    }

    if (proc->out == 255) {
        exit(0);
    }

    switch (proc->out)
    {
        case 0x01: {
            std::cout << proc->mem[proc->ma];
        } break;

        case 0x02: {
            while (proc->mem[proc->ma] != '\0') {
                std::cout << proc->mem[proc->ma];
                proc->ma++;
            }
        } break;

        case 0x03: {
            proc->in = getchar();
        } break;

        case 0x04: {
            std::string str = "";
            std::cin >> str;
            for (char c : str) {
                proc->mem[proc->ma] = c;
                proc->ma++;
            }
        } break;

        case 0x05: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x05;
        } break;

        case 0x06: {
            proc->mem[proc->ma] = getchar();
        } break;

        case 0x07: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x07;
        } break;

        case 0x09: {
            out_subfunc_args_num = 2;
            out_subfunc = out0x09;
        } break;

        case 0x0A: {
            proc->mem[proc->ma] = buffers->long_buf_index>>8;
            proc->ma++;
            proc->mem[proc->ma] = buffers->long_buf_index;
            proc->ma++;
        } break;

        case 0x0B: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x0B;
        } break;

        case 0x0C: {
            while (proc->mem[proc->ma] != '\0') {
                buffers->long_buf[buffers->long_buf_index] = proc->mem[proc->ma];
                buffers->long_buf_index++;
                proc->ma++;
            }
        } break;

        case 0x0D: {
            proc->in = buffers->long_buf[buffers->long_buf_index];
        } break;

        case 0x0E: {
            while (buffers->long_buf[buffers->long_buf_index] != 00) {
                proc->mem[proc->ma] = buffers->long_buf[buffers->long_buf_index];
                proc->ma++;
                buffers->long_buf_index++;
            }
        } break;

        case 0x21: {
            std::string disk_name = "";
    
            for (int i = 0; i < 65563; i++) {
                pp_info.disk[i] = 0;
            }

            std::cin >> disk_name;
            std::ifstream diskf(disk_name); {
                char b;
                for (int i = 0; diskf.get(b); i++) {
                    pp_info.disk[i] = b;
                }
            } diskf.close();
        } break;

        case 0x22: {} break;

        case 0x23: {
            out_subfunc_args_num = 2;
            out_subfunc = out0x23;
        } break;

        case 0x24: {
            proc->mem[proc->ma] = pp_info.disk_index>>8;
            proc->ma++;
            proc->mem[proc->ma] = pp_info.disk_index;
            proc->ma++;
        }

        case 0x25: {
            proc->mem[proc->ma] = pp_info.disk[pp_info.disk_index];
            proc->ma++;
        } break;

        case 0x26: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x26;
        } break;

        case 0x27: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x27;
        } break;

        case 0x28: {
            out_subfunc_args_num = 1;
            out_subfunc = out0x28;
        } break;
    }
}
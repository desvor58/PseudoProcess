#pragma once

#include "process.hpp"

void ops_exeting(Process *proc, i8 op)
{
    switch (op)
    {
        case operations::_ADD:
            proc->rg3 = proc->rg1 + proc->rg2;
        break;
        
        case operations::_SUB:
            proc->rg3 = proc->rg1 - proc->rg2;
        break;
        
        case operations::_AND:
            proc->rg3 = proc->rg1 & proc->rg2;
        break;
        
        case operations::_OR:
            proc->rg3 = proc->rg1 | proc->rg2;
        break;
        
        case operations::_XOR:
            proc->rg3 = proc->rg1 ^ proc->rg2;
        break;
        
        case operations::_NOT:
            proc->rg3 = !proc->rg1;
        break;
        
        case operations::_SHR:
            proc->rg3 = proc->rg1 >> proc->rg2;
        break;
        
        case operations::_SHL:
            proc->rg3 = proc->rg1 << proc->rg2;
        break;

        case operations::_INC:
            proc->ma++;
        break;

        case operations::_DEC:
            proc->ma--;
        break;
    }
}

u16 conds_executing(Process *proc, i8 op, i8 reg, i8 page, u8 jmpaddr)
{
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
    return 0;
}

void out_analis(Process *proc)
{
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
    }
}
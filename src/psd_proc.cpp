#include <iostream>
#include <fstream>
#include <vector>

#include "utils/types.hpp"

typedef          char   i8;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

class Args
{
public:
    std::string infile_name = "";
    bool dbg_mode           = false;
    bool endout             = false;

    Args(int argc, char **argv)
    {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-i") == 0) {
                infile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-dbg") == 0) {
                dbg_mode = true;
                continue;
            }
            if (strcmp(argv[i], "-endout") == 0) {
                endout = true;
                continue;
            }
            infile_name = argv[i];
        }
    }
};

static i8 rg0 = 0;
static i8 rg1 = 0;
static i8 rg2 = 0;
static i8 rg3 = 0;
static u8 ma  = 0;  // mem address
static u8 in  = 0;  // input
static u8 out = 0;  // output

static i8 mem[256] {};



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

void ops_exeting(i8 op)
{
    switch (op)
    {
        case operations::_ADD:
            set(registers::_rg3, (get(registers::_rg1) + get(registers::_rg2)));
        break;
        
        case operations::_SUB:
            set(registers::_rg3, (get(registers::_rg1) - get(registers::_rg2)));
        break;
        
        case operations::_AND:
            set(registers::_rg3, (get(registers::_rg1) & get(registers::_rg2)));
        break;
        
        case operations::_OR:
            set(registers::_rg3, (get(registers::_rg1) | get(registers::_rg2)));
        break;
        
        case operations::_XOR:
            set(registers::_rg3, (get(registers::_rg1) ^ get(registers::_rg2)));
        break;
        
        case operations::_NOT:
            set(registers::_rg3, !get(registers::_rg1));
        break;
        
        case operations::_SHR:
            set(registers::_rg3, (get(registers::_rg1) >> get(registers::_rg2)));
        break;
        
        case operations::_SHL:
            set(registers::_rg3, (get(registers::_rg1) << get(registers::_rg2)));
        break;

        case operations::_INC:
            set(registers::_ma, get(registers::_ma) + 1);
        break;

        case operations::_DEC:
            set(registers::_ma, get(registers::_ma) - 1);
        break;
    }
}

u16 conds_executing(i8 op, i8 reg, i8 page, u8 jmpaddr)
{
    switch (op)
    {
        case conditionals::_JMP:
            if (jmpaddr == 0) {
                return get(reg) * (page*256);
            }
            return jmpaddr + (page*256);
        break;

        case conditionals::_JEZ:
            if (get(reg) == 0) {
                return jmpaddr + (page*256);
            }
        break;

        case conditionals::_JGZ:
            if (get(reg) > 0) {
                return jmpaddr + (page*256);
            }
        break;

        case conditionals::_JLZ:
            if (get(reg) < 0) {
                return jmpaddr + (page*256);
            }
        break;
    }
    return 0;
}

void out_analis()
{
    if (out == 255) {
        exit(0);
    }

    switch (out)
    {
        case 0x01: {
            std::cout << mem[ma];
        } break;

        case 0x02: {
            while (mem[ma] != '\0') {
                std::cout << mem[ma];
                ma++;
            }
        } break;

        case 0x03: {
            in = getchar();
        } break;
    }
}

int main(int argc, char **argv)
{
    Args args(argc, argv);

    std::vector<u8> bytes {};

    std::ifstream file(args.infile_name, std::ios::binary);
    {
        char byte = ' ';
        while (file.get(byte)) {
            bytes.push_back(byte);
        }
    }
    file.close();

    system("cls");

    for (u16 i = 0; (i < bytes.size()); i++) {
        u8 byte = bytes[i];

        if (args.dbg_mode) {
            std::cout << "programm address: " << i << std::endl;
        }
        
        switch ((byte & 0b11000000)>>6) {
            case commands::_RGWRT: {
                i8 to = (byte & 0b00111000)>>3;
                set(to, bytes[++i]);
            } break;

            case commands::_MOV: {
                i8 from =  byte & 0b00000111;
                i8 to   = (byte & 0b00111000)>>3;
                set(to, get(from));
            } break;
            
            case commands::_OPERATIONS: {
                i8 op  = (byte & 0b00111100)>>2;
                ops_exeting(op);
            
            } break;
            
            case commands::_CONDITIONALS: {
                i8 op      = (byte & 0b00110000)>>4;
                i8 reg     = (byte & 0b00001100)>>2;
                i8 page    =  byte & 0b00000011;
                u8 jmpaddr =  bytes[++i];
                i = conds_executing(op, reg, page, jmpaddr) - 1;
            } break;
        }

        out_analis();

        if (args.dbg_mode) {
            std::cout << "rg0: " << +rg0 << std::endl;
            std::cout << "rg1: " << +rg1 << std::endl;
            std::cout << "rg2: " << +rg2 << std::endl;
            std::cout << "rg3: " << +rg3 << std::endl;
            std::cout << "ma:  " << +ma  << std::endl;
            std::cout << "in:  " << +in  << std::endl;
            std::cout << "out: " << +out << std::endl;

            std::cout << "jmp addr: " << i << std::endl;

            system("pause");
        }

        in  = 0;
        out = 0;
    }

    if (args.endout) {
        std::cout << "rg0: " << +rg0 << std::endl;
        std::cout << "rg1: " << +rg1 << std::endl;
        std::cout << "rg2: " << +rg2 << std::endl;
        std::cout << "rg3: " << +rg3 << std::endl;
        std::cout << "ma:  " << +ma  << std::endl;
        std::cout << "in:  " << +in  << std::endl;
        std::cout << "out: " << +out << std::endl;
    }

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include "utils/types.hpp"
#include "process.hpp"
#include "funcs.hpp"

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

static PPbuffers buffers {};
static Process   current_proc;
static u8        disk[65563] = {};

static PPinfo pp_info {&current_proc, &buffers, disk, 0};

int main(int argc, char **argv)
{
    Args args(argc, argv);

    std::ifstream file(args.infile_name, std::ios::binary);
    {
        char byte = ' ';
        while (file.get(byte)) {
            buffers.prgm_buf[buffers.prgm_buf_index] = byte;
            buffers.prgm_buf_index++;
        }
    }
    file.close();

    system("cls");
    for (buffers.prgm_buf_index = 0; buffers.prgm_buf_index < 65535; buffers.prgm_buf_index++) {
        u8 byte = buffers.prgm_buf[buffers.prgm_buf_index];

        if (args.dbg_mode) {
            std::cout << "programm address: " << buffers.prgm_buf_index << std::endl;
        }
        
        switch ((byte & 0b11000000)>>6) {
            case commands::_RGWRT: {
                i8 to = (byte & 0b00111000)>>3;
                current_proc.set(to, buffers.prgm_buf[++buffers.prgm_buf_index]);
            } break;

            case commands::_MOV: {
                i8 from =  byte & 0b00000111;
                i8 to   = (byte & 0b00111000)>>3;
                current_proc.set(to, current_proc.get(from));
            } break;
            
            case commands::_OPERATIONS: {
                i8 op  = (byte & 0b00111100)>>2;
                ops_exeting(pp_info, op);
            } break;
            
            case commands::_CONDITIONALS: {
                i8 op      = (byte & 0b00110000)>>4;
                i8 reg     = (byte & 0b00001100)>>2;
                i8 page    =  byte & 0b00000011;
                u8 jmpaddr =  buffers.prgm_buf[++buffers.prgm_buf_index];
                buffers.prgm_buf_index = conds_executing(pp_info, op, reg, page, jmpaddr) - 1;
            } break;
        }

        out_analis(pp_info);

        if (args.dbg_mode) {
            std::cout << "rg0: " << +current_proc.rg0 << std::endl;
            std::cout << "rg1: " << +current_proc.rg1 << std::endl;
            std::cout << "rg2: " << +current_proc.rg2 << std::endl;
            std::cout << "rg3: " << +current_proc.rg3 << std::endl;
            std::cout << "ma:  " << +current_proc.ma  << std::endl;
            std::cout << "in:  " << +current_proc.in  << std::endl;
            std::cout << "out: " << +current_proc.out << std::endl;

            for (int i = 0; i < 32; i++) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << +current_proc.mem[current_proc.ma + i] << std::dec << ' ';
            }
            std::cout << std::endl;
            for (int i = 0; i < 32; i++) {
                std::cout << current_proc.mem[current_proc.ma + i] << "  ";
            }
            std::cout << std::endl;

            std::cout << "next addr: " << buffers.prgm_buf_index+1 << std::endl;

            getchar();
        }

        current_proc.in  = 0;
        current_proc.out = 0;
    }

    if (args.endout) {
        std::cout << "rg0: " << +current_proc.rg0 << std::endl;
        std::cout << "rg1: " << +current_proc.rg1 << std::endl;
        std::cout << "rg2: " << +current_proc.rg2 << std::endl;
        std::cout << "rg3: " << +current_proc.rg3 << std::endl;
        std::cout << "ma:  " << +current_proc.ma  << std::endl;
        std::cout << "in:  " << +current_proc.in  << std::endl;
        std::cout << "out: " << +current_proc.out << std::endl;
    }

    return 0;
}
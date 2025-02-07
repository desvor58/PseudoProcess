#include <iostream>
#include <fstream>
#include <vector>

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

    Process main_proc;

    system("cls");

    for (u16 i = 0; (i < bytes.size()); i++) {
        u8 byte = bytes[i];

        if (args.dbg_mode) {
            std::cout << "programm address: " << i << std::endl;
        }
        
        switch ((byte & 0b11000000)>>6) {
            case commands::_RGWRT: {
                i8 to = (byte & 0b00111000)>>3;
                main_proc.set(to, bytes[++i]);
            } break;

            case commands::_MOV: {
                i8 from =  byte & 0b00000111;
                i8 to   = (byte & 0b00111000)>>3;
                main_proc.set(to, main_proc.get(from));
            } break;
            
            case commands::_OPERATIONS: {
                i8 op  = (byte & 0b00111100)>>2;
                ops_exeting(&main_proc, op);
            
            } break;
            
            case commands::_CONDITIONALS: {
                i8 op      = (byte & 0b00110000)>>4;
                i8 reg     = (byte & 0b00001100)>>2;
                i8 page    =  byte & 0b00000011;
                u8 jmpaddr =  bytes[++i];
                i = conds_executing(&main_proc, op, reg, page, jmpaddr) - 1;
            } break;
        }

        out_analis(&main_proc);

        // for (int j = 0; j < 10; j++) {
        //     std::cout << "mem " << j << " " << +main_proc.mem[j] << std::endl;
        // }

        if (args.dbg_mode) {
            std::cout << "rg0: " << +main_proc.rg0 << std::endl;
            std::cout << "rg1: " << +main_proc.rg1 << std::endl;
            std::cout << "rg2: " << +main_proc.rg2 << std::endl;
            std::cout << "rg3: " << +main_proc.rg3 << std::endl;
            std::cout << "ma:  " << +main_proc.ma  << std::endl;
            std::cout << "in:  " << +main_proc.in  << std::endl;
            std::cout << "out: " << +main_proc.out << std::endl;

            std::cout << "jmp addr: " << i << std::endl;

            system("pause");
        }

        main_proc.in  = 0;
        main_proc.out = 0;
    }

    if (args.endout) {
        std::cout << "rg0: " << +main_proc.rg0 << std::endl;
        std::cout << "rg1: " << +main_proc.rg1 << std::endl;
        std::cout << "rg2: " << +main_proc.rg2 << std::endl;
        std::cout << "rg3: " << +main_proc.rg3 << std::endl;
        std::cout << "ma:  " << +main_proc.ma  << std::endl;
        std::cout << "in:  " << +main_proc.in  << std::endl;
        std::cout << "out: " << +main_proc.out << std::endl;
    }

    return 0;
}
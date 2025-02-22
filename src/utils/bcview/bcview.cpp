#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "../delib.hpp"
#include "../types.hpp"

std::string getr(u8 num)
{
    for (auto p : regs) {
        if (num == p.second) {
            return p.first;
        }
    }
    return "";
}
std::string geto(u8 num)
{
    for (auto p : ops) {
        if (num == p.second) {
            return p.first;
        }
    }
    return "";
}
std::string getcnd(u8 num)
{
    for (auto p : conds) {
        if (num == p.second) {
            return p.first;
        }
    }
    return "";
}

struct jmp_line_t
{
    bool reverced;
    u16 jmpa;
    u16 trga;
};

static std::vector<jmp_line_t> jmp_lines {};

int main(int argc, char **argv)
{
    std::ifstream file(argv[1], std::ios::binary);
    
    std::vector<u8> bytes {};
    i8              byte = 0;
    
    while (file.get(byte)) {
        bytes.push_back(byte);
    }

    file.close();

    u8 bcomm;
    u8 barg1;
    u8 barg2;
    u8 barg3;
    u8 bjmpa;

    jmp_line_t jmp_line;

    bool draw_jmp_line = 0;

    for (int i = 0; i < bytes.size(); i++) {
        byte = bytes[i];

        bcomm = (byte & 0b11000000) >> 6;

        if (bcomm == 0b00) {
            i++;
            continue;
        }

        if (bcomm == 0b11) {
            jmp_line.jmpa = i;
            jmp_line.trga = bytes[++i];
            jmp_line.reverced = jmp_line.jmpa > jmp_line.trga;\

            std::cout << jmp_line.jmpa << " " << jmp_line.trga << " " << jmp_line.reverced << std::endl;

            jmp_lines.push_back(jmp_line);
        }
    }

    for (int i = 0; i < bytes.size(); i++) {
        byte = bytes[i];

        bcomm = (byte & 0b11000000) >> 6;

        std::cout
            << std::hex << "0x" << std::setw(4) << std::setfill('0') << i
            << " "
            << std::dec << std::setw(5) << std::setfill('0') << i << "    ";

        switch (bcomm)
        {
        case 0b00:
            barg1 = (byte & 0b00111000) >> 3;
            barg2 = bytes[++i];
            std::cout << "wrt" << " " << getr(barg1) << " " << +barg2 << std::endl;
        break;

        case 0b01:
            barg1 = (byte & 0b00111000) >> 3;
            barg2 = (byte & 0b00000111);
            std::cout << "mov" << " " << getr(barg1) << " " << getr(barg2) << std::endl;
        break;

        case 0b10:
            barg1 = (byte & 0b00111100) >> 2;
            std::cout << geto(barg1) << std::endl;
        break;

        case 0b11:
            barg1 = (byte & 0b00110000) >> 4;
            barg2 = (byte & 0b00001100) >> 2;
            barg3 =  byte & 0b00000011;
            bjmpa =  bytes[++i];
            std::cout << getcnd(barg1) << " " << getr(barg2) << " " << +barg3 << " " << +bjmpa << std::endl;
        break;
        
        default: break;
        }
    }

    // std::cout << (char)218 << '>' << std::endl;
    // std::cout << (char)179 << std::endl;
    // std::cout << (char)179 << std::endl;
    // std::cout << (char)179 << std::endl;
    // std::cout << (char)192 << (char)196 << std::endl;

    return 0;
}


//     ...
// |-->0x0002 00002 wrt rg0 5
// |   0x0003 00003 wrt rg1 3
// |   ...
// |---0x000A 00010 jmp rg0 0 2


#pragma once

#include <string>

#include "../types.hpp"
#include "../delib.hpp"
#include "tbc_args.hpp"

std::vector<i8> process(Args args, std::vector<vectok> lines)
{
    std::vector<i8> res {};
    u8 byte = 0;

    // цикл по строкам
    for (vectok line : lines) {
        if (line.empty()) continue;

        if (comms.count(line[0]) > 0) {
            switch (comms[line[0]])
            {
                case commands::_RGWRT: {
                    byte =  comms[line[0]]  << 6;
                    byte += regs[line[1]] << 3;
                    res.push_back(byte);
                    byte = std::stoi(line[2]);
                    res.push_back(byte);
                } break;

                case commands::_MOV: {
                    byte =  comms[line[0]]  << 6;
                    byte += regs[line[1]] << 3;
                    byte += regs[line[2]];
                    res.push_back(byte);
                } break;

                case commands::_OPERATIONS: {
                    byte =  0b10000000;
                    byte += ops[line[0]] << 2;
                    res.push_back(byte);
                } break;

                case commands::_CONDITIONALS: {
                    byte =  0b11000000;
                    byte += conds[line[0]] << 4;
                    if (conds[line[0]] == conditionals::_JMP) {
                        byte += std::stoi(line[2]);
                        res.push_back(byte);
                        byte = std::stoi(line[1]);
                        res.push_back(byte);
                        break;
                    }
                    byte += regs[line[1]]  << 2;
                    byte += std::stoi(line[3]);
                    res.push_back(byte);
                    byte = std::stoi(line[2]);
                    res.push_back(byte);
                } break;

                default: break;
            }
        }
    }

    return res;
}
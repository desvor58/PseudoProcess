#pragma once

#include <iostream>
#include <vector>

#include "../types.hpp"
#include "tbc_args.hpp"
#include "../delib.hpp"

struct preprocessor_ret
{
    part lines;
};

struct label_info
{
    u16 addr;
    u8  page;
};

std::map<std::string, vectok>     defines   = {};
std::map<std::string, label_info> labels    = {};
std::map<std::string, part>       inlines   = {};
std::map<std::string, part>       programms = {};

vectok preprocess_defines_parse(Args args, vectok line);
part preprocess_commands_decl(Args args, part lines, int index);
part preprocess_commands_parse(Args args, part lines, int index);

preprocessor_ret preprocess(Args args, vectok lines)
{
    preprocessor_ret res {};

    part slines {};

    u16 byte_counter = 0;

    // lines to slines loop
    if (args.log_preproc_loops) std::cout << "lines to slines loop" << std::endl;
        for (std::string line : lines) {
            line = de::toc(de::trim(line), ';');
            if (line.empty()) continue;

            vectok ss = de::split(de::trim(line), ' ');
            for (int i = 0; i < ss.size(); i++) {
                if (ss[i].empty()) ss.erase(ss.begin() + i);
                ss[i] = de::trim(ss[i]);
            }
            slines.push_back(ss);
        }
    if (args.log_preproc_loops) std::cout << "lines to slines loop end" << std::endl;

    // preproc defines decl loop
    if (args.log_preproc_loops) std::cout << "preproc defines decl loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];

            bool ret = false;
            if (line[0] == "define") {
                defines[line[1]] = de::slise(line, 2, -1);
                ret = true;
            }

            if (ret) slines.erase(slines.begin() + i);
        }
    if (args.log_preproc_loops) std::cout << "preproc defines decl loop end" << std::endl;

    // preproc defines parse loop
    if (args.log_preproc_loops) std::cout << "preproc defines parse loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];
            
            slines[i] = preprocess_defines_parse(args, line);
        }
    if (args.log_preproc_loops) std::cout << "preproc defines parse loop end" << std::endl;

    // preproc commands decl loop
    if (args.log_preproc_loops) std::cout << "preproc commands decl loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];
            
            part ret = preprocess_commands_decl(args, slines, i);

            if (!ret.empty()) {
                part start_part = de::slise(slines, 0, i);
                part end_part   = de::slise(slines, i+1, -1);
                part new_part   = ret;
                
                part new_slines = {};

                de::push_vec(&new_slines, start_part);
                de::push_vec(&new_slines, new_part);
                de::push_vec(&new_slines, end_part);

                slines = new_slines;
            }
        }
    if (args.log_preproc_loops) std::cout << "preproc commands decl loop end" << std::endl;

    // preproc commands parse loop
    if (args.log_preproc_loops) std::cout << "preproc commands parse loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];

            part start_part = de::slise(slines, 0, i);
            part end_part   = de::slise(slines, i+1, -1);
            part new_part   = preprocess_commands_parse(args, slines, i);
        
            part new_slines = {};

            de::push_vec(&new_slines, start_part);
            de::push_vec(&new_slines, new_part);
            de::push_vec(&new_slines, end_part);

            slines = new_slines;
        }
    if (args.log_preproc_loops) std::cout << "preproc commands parse loop end" << std::endl;

    // labels declaration loop
    if (args.log_preproc_loops) std::cout << "labels decl loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];

            if (line[0][line[0].size()-1] == ':') {
                labels[de::signore(line[0], ':')]
                    = {(u16)(byte_counter % 256), (u8)(byte_counter / 256)};

                slines[i].erase(slines[i].begin());
            }

            if (comms.count(slines[i][0]) > 0) {
                if (comms[slines[i][0]] == commands::_RGWRT
                || comms[slines[i][0]] == commands::_CONDITIONALS) {
                    byte_counter += 2;
                }
                if (comms[slines[i][0]] == commands::_MOV
                || comms[slines[i][0]] == commands::_OPERATIONS)
                {
                    byte_counter++;
                }
            }
        }
    if (args.log_preproc_loops) std::cout << "labels decl loop end" << std::endl;
    
    // labels parse loop
    if (args.log_preproc_loops) std::cout << "labels parse loop" << std::endl;
        for (int i = 0; i < slines.size(); i++) {
            vectok line = slines[i];

            for (int j = 0; j < line.size(); j++) {
                if (labels.count(line[j]) > 0) {
                    slines[i][j] = std::to_string(labels[line[j]].addr);
                    slines[i].push_back(std::to_string(labels[line[j]].page));
                }
            }
        }
    if (args.log_preproc_loops) std::cout << "labels parse loop end" << std::endl;
    
    res.lines = slines;

    return res;
}

vectok preprocess_defines_parse(Args args, vectok line)
{
    vectok res = line;

    for (int i = 0; i < res.size(); i++) {
        if (defines.count(res[i]) > 0) {
            vectok start = de::slise(res, 0, i);
            vectok end_p = de::slise(res, i+1, -1);
            vectok new_p = defines[res[i]];

            res.clear();
            de::push_vec(&res, start);
            de::push_vec(&res, new_p);
            de::push_vec(&res, end_p);
        }
    }

    return res;
}

part preprocess_commands_decl(Args args, part lines, int index)
{
    vectok line = lines[index];

    part res = {};

    if (line[0] == "inline") {
        part body {};

        token name = line[1];
        token oper = "";
        if (line.size() >= 3) oper = line[2];
        else                  oper = lines[++index][0];

        for (int i = ++index; lines[i][0] != "end"; i++) {
            body.push_back(lines[i]);
        }

        inlines[name] = body;
    }

    return res;
}

part preprocess_commands_parse(Args args, part lines, int index)
{
    vectok line = lines[index];

    part res = {};

    if (inlines.count(line[0]) > 0) {
        res = inlines[line[0]];
    }
    else if (line[0] == "out") {
        for (std::string p : de::slise(line, 1, -1)) {
            res.push_back({"wrt", "out", p});
        }
    }
    else if (line[0] == "push") {
        bool bs = false;
        for (char c : de::merger(de::slise(line, 1, -1), ' ')) {
            if (bs) {
                if (c == '\\') {
                    res.push_back({"wrt", "mem", std::to_string('\\')});
                    res.push_back({"inc"});
                }
                if (c == 'n') {
                    res.push_back({"wrt", "mem", std::to_string('\n')});
                    res.push_back({"inc"});
                }
                if (c == '0') {
                    res.push_back({"wrt", "mem", std::to_string('\0')});
                    res.push_back({"inc"});
                }
                continue;
            }
            if (c == '\\') {
                bs = true;
                continue;
            }
            res.push_back({"wrt", "mem", std::to_string(c)});
            res.push_back({"inc"});
        }
    }
    else {
        res = {line};
    }

    return res;
}
#pragma once

#include <iostream>
#include <vector>

#include "../types.hpp"
#include "tbc_args.hpp"
#include "../../delib.hpp"

struct preprocessor_ret
{
    std::vector<std::vector<std::string>> lines;
};

struct label_info
{
    u16 addr;
    u8  page;
};

std::map<std::string, std::vector<std::string>> defines = {};
std::map<std::string, label_info>               labels  = {};

std::vector<std::string> preprocess_defines_parse(Args args, std::vector<std::string> line);
std::vector<std::vector<std::string>> preprocess_commands_parse(Args args, std::vector<std::string> line);

preprocessor_ret preprocess(Args args, std::vector<std::string> lines)
{
    preprocessor_ret res {};

    std::vector<std::vector<std::string>> slines {};

    u16 byte_counter = 0;

    // lines to slines loop
    if (args.log_preproc_loops) std::cout << "lines to slines loop" << std::endl;
    for (std::string line : lines) {
        line = de::toc(de::trim(line), ';');
        if (line.empty()) continue;

        std::vector<std::string> ss = de::split(de::trim(line), ' ');
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
        std::vector<std::string> line = slines[i];

        bool ret = false;
        if (line[0][0] == '#') {
            if (de::signore(line[0], '#') == "define") {
                defines[de::signore(line[1], '#')] = de::slise(line, 2, -1);
                ret = true;
            }
        }

        if (ret) slines.erase(slines.begin() + i);
    }
    if (args.log_preproc_loops) std::cout << "preproc defines decl loop end" << std::endl;

    // preproc defines parse loop
    if (args.log_preproc_loops) std::cout << "preproc defines parse loop" << std::endl;
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];
        
        slines[i] = preprocess_defines_parse(args, line);
    }
    if (args.log_preproc_loops) std::cout << "preproc defines parse loop end" << std::endl;

    // preproc commands parse loop
    if (args.log_preproc_loops) std::cout << "preproc commands parse loop" << std::endl;
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];

        std::vector<std::vector<std::string>> start_part = de::slise(slines, 0, i);
        std::vector<std::vector<std::string>> end_part   = de::slise(slines, i+1, -1);
        std::vector<std::vector<std::string>> new_part   = preprocess_commands_parse(args, line);
    
        std::vector<std::vector<std::string>> new_slines = {};

        de::push_vec(&new_slines, start_part);
        de::push_vec(&new_slines, new_part);
        de::push_vec(&new_slines, end_part);

        slines = new_slines;
    }
    if (args.log_preproc_loops) std::cout << "preproc commands parse loop end" << std::endl;

    // labels declaration loop
    if (args.log_preproc_loops) std::cout << "labels decl loop" << std::endl;
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];

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
        std::vector<std::string> line = slines[i];

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

std::vector<std::string> preprocess_defines_parse(Args args, std::vector<std::string> line)
{
    std::vector<std::string> res = line;

    for (int i = 0; i < res.size(); i++) {
        if (defines.count(res[i]) > 0) {
            res = defines[res[i]];
        }
    }

    return res;
}

std::vector<std::vector<std::string>> preprocess_commands_parse(Args args, std::vector<std::string> line)
{
    std::vector<std::vector<std::string>> res = {};

    if (line[0] == "inline") {
        
    }

    if (line[0] == "push") {
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
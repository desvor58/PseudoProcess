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

bool preprocess_commands(Args args, std::vector<std::string> line);
std::vector<std::string> preprocess_commands_parse(Args args, std::vector<std::string> line);
void preprocess_labels_parse(Args args, std::vector<std::string> line);

preprocessor_ret preprocess(Args args, std::vector<std::string> lines)
{
    preprocessor_ret res {};

    std::vector<std::vector<std::string>> slines {};

    u16 byte_counter = 0;

    // lines to slines loop
    for (std::string line : lines) {
        line = de::toc(line, ';');
        if (line.empty()) continue;

        std::vector<std::string> ss = de::split(de::trim(line), ' ');
        for (int i = 0; i < ss.size(); i++) {
            if (ss[i].empty()) ss.erase(ss.begin() + i);
            ss[i] = de::trim(ss[i]);
        }
        slines.push_back(ss);
    }

    // preproc commands decl loop
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];

        bool ret = preprocess_commands(args, line);
        if (ret) slines.erase(slines.begin() + i);
    }

    // preproc command parse loop
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];
        
        slines[i] = preprocess_commands_parse(args, line);
    }

    // labels declaration loop
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
    
    for (int i = 0; i < slines.size(); i++) {
        std::vector<std::string> line = slines[i];

        for (int j = 0; j < line.size(); j++) {
            if (labels.count(line[j]) > 0) {
                slines[i][j] = std::to_string(labels[line[j]].addr);
                slines[i].push_back(std::to_string(labels[line[j]].page));
            }
        }
    }

    res.lines = slines;

    return res;
}

std::vector<std::string> preprocess_commands_parse(Args args, std::vector<std::string> line)
{
    std::vector<std::string> res = line;

    for (int i = 0; i < res.size(); i++) {
        if (defines.count(res[i]) > 0) {
            res = defines[res[i]];
        }
    }

    return res;
}

bool preprocess_commands(Args args, std::vector<std::string> line)
{
    if (line[0][0] == '#') {
        if (de::signore(line[0], '#') == "define") {
            defines[de::signore(line[1], '#')] = de::slise(line, 2, 0);
            return true;
        }
    }
    return false;
}
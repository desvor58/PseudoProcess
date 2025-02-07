#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "../types.hpp"
#include "tbc_args.hpp"
#include "../../delib.hpp"

#include "preprocessor.hpp"
#include "processor.hpp"

int main(int argc, char **argv)
{
    Args args(argc, argv);

    std::vector<std::string> file_lines {};
    std::ifstream ifile(args.infile_name);
    if (!ifile.is_open()) {
        std::cerr << "Error: Input file not be open (" << args.infile_name << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    char c = ' ';
    std::string str = "";
    while (ifile.get(c)) {
        if (c == '\n') {
            if (!str.empty()) {
                file_lines.push_back(str);
                str = "";
            }
            continue;
        }
        str += c;
    }
    if (!str.empty()) file_lines.push_back(str);
    ifile.close();


    preprocessor_ret preproc_ret = preprocess(args, file_lines);

    std::cout << std::endl << "PROCESSED PARTS" << std::endl;

    for (vectok toks : preproc_ret.lines) {
        for (token s : toks) {
            std::cout << s << " END ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << std::endl << "PROCESS" << std::endl;
    std::vector<i8> bytes = process(args, preproc_ret.lines);
    std::cout << "END PROCESS" << std::endl;


    std::ofstream ofile(args.outfile_name, std::ios::out | std::ios::binary);
    if (!ofile.is_open()) {
        std::cerr << "Error: Output file not be open" << std::endl;
    }
    ofile.write(&bytes[0], bytes.size());
    ofile.close();

    return 0;
}
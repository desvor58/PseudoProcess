#pragma once

#include <iostream>

class Args
{
public:
    std::string infile_name       = "";
    std::string outfile_name      = "a.bc";
    bool        log_preproc_loops = false;

    Args(int argc, char **argv)
    {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-i") == 0) {
                infile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-o") == 0) {
                outfile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-logloops") == 0) {
                log_preproc_loops = true;
                continue;
            }
            infile_name = argv[i];
        }
    }
};
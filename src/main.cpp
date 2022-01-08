/*
Copyright (C) 2021  Kasyanov Nikolay Alexeevich (Unbewohnte (me@unbewohnte.xyz))

This file is part of broom.

broom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

broom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with broom.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <stdexcept>
#include <string.h>
#include <vector>

#include "entry.hpp"
#include "broom.hpp"

// Broom version number
#define VERSION "v0.1.1"

void print_help() {
    std::cout
    << "broom [FLAGS..] [COMMAND] [DIRECTORY]" << std::endl << std::endl
    << "[FLAGS]" << std::endl
    << "-v | --version -> print version information and exit" << std::endl
    << "-h | --help -> print this message and exit" << std::endl << std::endl

    << "[COMMANDS]" << std::endl
    << "sweep -> scan for duplicate files and delete (sweep) all of them but the last one" << std::endl
    << "scan -> scan for duplicate files and output information in a file" << std::endl
    << std::endl;
};

void print_version() {
    std::cout
    << "broom " << VERSION << std::endl
    << "incurable hoarder`s helpful friend" << std::endl << std::endl

    << "Copyright (C) 2021  Kasyanov Nikolay Alexeevich (Unbewohnte (me@unbewohnte.xyz))" << std::endl
    << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
    << "This is free software, and you are welcome to redistribute it" << std::endl
    << "under certain conditions" << std::endl
    << std::endl;
};

int main(int argc, char* argv[]) {
    broom::Options options;
    std::filesystem::path tracked_path;

    if (argc < 2) {
        print_help();
        return 0;
    };

    // process command line arguments
    for (unsigned int i = 1; i < argc; i++) {
        // flags -> command -> directory

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--benchmark") == 0) {
            options.benchmarking = true;
        }
        else if (strcmp(argv[i], "sweep") == 0) {
            options.sweeping = true;
        }
        else if (strcmp(argv[i], "scan") == 0) {
            options.sweeping = false;
        }
        else {
            // add path
            tracked_path = std::filesystem::path(argv[i]);
        };
    };

    // no path was specified at all
    if (tracked_path.empty()) {
        print_help();
        return 1;
    };


    broom::Broom broom(options);
    try {
        broom.track(tracked_path);
        broom.scan();
        broom.create_scan_results_list();
    } catch(const std::invalid_argument& e) {
        std::cerr
        << "[ERROR] Invalid argument: " << std::endl
        << e.what() << std::endl;
        return 1;

    } catch(const std::filesystem::filesystem_error& e) {
        std::cerr
        << "[ERROR] FS error: " << std::endl
        << e.what() << std::endl;
        return 1;

    } catch(...) {
        std::cerr
        << "[ERROR] Unexpected exception" << std::endl;
        return 1;
    };

    return 0;
};

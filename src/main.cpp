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
#include <future>
#include <algorithm>

#include "entry.hpp"
#include "broom.hpp"

// Broom version number
#define VERSION "v0.2.0"

void print_help() {
    std::cout
    << "broom [FLAGS..] [COMMAND] [DIRECTORY]" << std::endl << std::endl
    << "[FLAGS]" << std::endl
    << "-v | --version -> print version information and exit" << std::endl
    << "-h | --help -> print this message and exit" << std::endl << std::endl

    << "[COMMANDS]" << std::endl
    << "sweep -> scan for duplicate files, save results in a file and REMOVE empty files" << std::endl
    << "scan -> scan and save results in a file without touching any files [DEFAULT]" << std::endl
    << std::endl;
};

void print_version() {
    std::cout
    << "broom " << VERSION << std::endl
    << "incurable hoarder`s helpful friend" << std::endl << std::endl

    << "          _" << std::endl
    << "         //" << std::endl
    << "        // " << std::endl
    << "       //  " << std::endl
    << "      //   " << std::endl
    << "   /####/  " << std::endl
    << "  //////   " << std::endl
    << " ///////   " << std::endl << std::endl

    << "Copyright (C) 2021  Kasyanov Nikolay Alexeevich (Unbewohnte (me@unbewohnte.xyz))" << std::endl
    << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
    << "This is free software, and you are welcome to redistribute it" << std::endl
    << "under certain conditions" << std::endl
    << std::endl;
};


int main(int argc, char* argv[]) {
    bool sweeping = false;

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
        else if (strcmp(argv[i], "sweep") == 0) {
            sweeping = true;
        }
        else if (strcmp(argv[i], "scan") == 0) {
            sweeping = false;
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


    broom::Broom broom;
    try {
        // auto t0 = std::chrono::high_resolution_clock::now();
        /*
        auto tracking_time = std::chrono::high_resolution_clock::now();

        std::cout
        << "[BENCHMARK] Tracking took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(tracking_time - t0).count()
        << " ms" << std::endl;
        */

        std::cout
        << "          _" << std::endl
        << "         //" << std::endl
        << "        // " << std::endl
        << "       //  " << std::endl
        << "      //   " << std::endl
        << "   /####/  " << std::endl
        << "  //////   " << std::endl
        << " ///////   " << std::endl << std::endl;
        if (sweeping) {
            std::cout << "[Sweeping]" << std::endl << std::endl;
        } else {
            std::cout << "[Scanning]" << std::endl << std::endl;
        }

        // track files in a given directory
        std::vector<entry::Entry> tracked_entries = broom.track(tracked_path);
        std::cout << "[INFO] Tracking " << tracked_entries.size() << " files" << std::endl;

        // find empty files
        uintmax_t empty_files = broom.find_empty_files(tracked_entries);
        std::cout << "[INFO] Found " << empty_files << " empty files" << std::endl;

        // if sweeping - remove empty files right away
        if (sweeping) {
            uintmax_t removed = broom.remove_empty_files(tracked_entries);
            std::cout << "[INFO] Removed " << removed << " empty files" << std::endl;
        }

        // untrack unique sizes
        uintmax_t untracked = broom.untrack_unique_sizes(tracked_entries);
        std::cout << "[INFO] Untracked " << untracked << " files with a unique size" << std::endl;

        // get content pieces for each entry
        tracked_entries.erase(std::remove_if(tracked_entries.begin(), tracked_entries.end(), [](entry::Entry& entry) -> bool {
            // ignore possible "permission denied"s
            try {
                entry.get_pieces();
                return false;
            } catch(...) {
                return true;
            }
        }), tracked_entries.end());


        // untrack unique contents
        untracked = broom.untrack_unique_contents(tracked_entries);
        std::cout << "[INFO] Untracked " << untracked << " files with unique contents" << std::endl;

        // mark entries as duplicates
        broom.mark_as_duplicates(tracked_entries);

        std::cout << "[INFO] " << tracked_entries.size() << " files left being tracked" << std::endl;

        auto grouped_duplicates = broom.group_duplicates(tracked_entries);

        // now only files with a non-unique size and contents are being tracked
        // are they REALLY duplicates ?
        // better to leave the REALL cleanup for the user, saving these entries in a file, than doing a blind and possibly destructive purge
        broom.create_scan_results_list(grouped_duplicates );
        std::cout << "[INFO] Created scan results file" << std::endl;

    } catch(const std::exception& e) {
        std::cerr
        << "[ERROR] " << e.what() << std::endl;
        return 1;
    };

    return 0;
};

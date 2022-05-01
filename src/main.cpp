/*
Copyright (C) 2021  Kasyanov Nikolay Alexeyevich (Unbewohnte (me@unbewohnte.xyz))

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
#define VERSION "v0.3.1"

void print_help() {
    std::cout
    << "broom [FLAGS..] [COMMAND] [DIRECTORY]\n\n"
    << "[FLAGS]\n"
    << "-v  | --version -> print version information and exit\n"
    << "-h  | --help -> print this message and exit\n"
    << "-ie | --ignore-empty -> do not remove empty files when sweeping\n"
    << "-od | --output-directory -> path to the directory to save results file in when scanning\n\n"

    << "[COMMANDS]\n"
    << "sweep -> scan for duplicate files, REMOVE empty files and REPLACE other duplicates with symlinks\n"
    << "scan -> scan and save results in a file without removing anything [DEFAULT]\n\n"

    << "[DIRECTORY]\n"
    << "path to the directory to be scanned\n\n";
};

void print_version() {
    std::cout
    << "broom " << VERSION << "\n"
    << "incurable hoarder`s helpful friend\n\n"

    << "          _\n" 
    << "         //\n" 
    << "        // \n" 
    << "       //  \n" 
    << "      //   \n" 
    << "   /####/  \n" 
    << "  //////   \n" 
    << " ///////   \n\n"

    << "Copyright (C) 2021  Kasyanov Nikolay Alexeyevich (Unbewohnte (me@unbewohnte.xyz))\n"
    << "This program comes with ABSOLUTELY NO WARRANTY.\n"
    << "This is free software, and you are welcome to redistribute it\n"
    << "under certain conditions\n";
};


int main(int argc, char* argv[]) {
    std::filesystem::path results_file_dir_path = ".";
    std::filesystem::path tracked_path;
    bool sweeping = false;
    bool ignore_empty = false;

    if (argc < 2) {
        print_help();
        return 0;
    };

    // process command line arguments
    for (unsigned int i = 1; i < (unsigned int) argc; i++) {
        // flags -> command -> directory

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
        else if (strcmp(argv[i], "-od") == 0 || strcmp(argv[i], "--output-directory") == 0) {
            i++;
            results_file_dir_path = std::filesystem::path(argv[i]);
        }
        else if (strcmp(argv[i], "-ie") == 0 || strcmp(argv[i], "--ignore-empty") == 0) {
            ignore_empty = true;
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
        std::cout
        << "          _\n"
        << "         //\n"
        << "        // \n"
        << "       //  \n"
        << "      //   \n"
        << "   /####/  \n"
        << "  //////   \n"
        << " ///////   \n\n";
        if (sweeping) {
            std::cout << "[Sweeping]\n\n";
        } else {
            std::cout << "[Scanning]\n\n";
        }

        // track files in a given directory
        std::vector<entry::Entry> tracked_entries = broom.track(tracked_path);
        std::cout << "[INFO] Tracking " << tracked_entries.size() << " files\n";

        // find empty files
        uintmax_t empty_files = broom.find_empty_files(tracked_entries);
        std::cout << "[INFO] Found " << empty_files << " empty files\n";

        // if sweeping - remove empty files right away
        if (sweeping && !ignore_empty) {
            uintmax_t removed = broom.remove_empty_files(tracked_entries);
            std::cout << "[INFO] Removed " << removed << " empty files\n";
        } else {
            // just untrack them, do not remove
            uintmax_t untracked_empty = broom.untrack_group(tracked_entries, entry::Group::EMPTY);
            std::cout << "[INFO] Skipped " << untracked_empty << " empty files\n";
        }

        // untrack unique sizes
        uintmax_t untracked = broom.untrack_unique_sizes(tracked_entries);
        std::cout << "[INFO] Untracked " << untracked << " files with a unique size\n";

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
        std::cout << "[INFO] Untracked " << untracked << " files with unique contents\n";

        // mark entries as duplicates
        broom.mark_as_duplicates(tracked_entries);

        std::cout << "[INFO] " << tracked_entries.size() << " files left being tracked\n";

        if (tracked_entries.size() == 0) {
            // No duplicates at all !
            std::cout << "[INFO] Nothing I can help with ! Congratulations !\n";
            return 0;
        }

        // make duplicate groups from all this mess that tracked_entries right now are
        auto grouped_duplicates = broom.group_duplicates(tracked_entries);

        double could_be_freed = 0;
        for (auto& record : grouped_duplicates) {
            could_be_freed += record.second[0].filesize * (record.second.size() - 1);
        }

        if (!sweeping) {
            // output a little information about how much space could be freed if every duplicate
            // in the group will be deleted but one
            std::cout <<"[INFO] " << could_be_freed / 1024 / 1024 << " MB could be freed\n";

            broom.create_scan_results_list(grouped_duplicates, results_file_dir_path);
            std::cout << "[INFO] Created scan results file\n";

        } else {
            // remove duplicates and create symlinks
            std::cout << "[INFO] Removing duplicates and creating symlinks...\n";
            broom.remove_duplicates_make_symlinks(grouped_duplicates);
            std::cout <<"[INFO] Freed approximately " << could_be_freed / 1024 / 1024 << " MB (May be incorrect)\n";
        }

    } catch(const std::exception& e) {
        std::cerr
        << "[ERROR] " << e.what() <<"\n";
        return 1;
    };

    return 0;
};

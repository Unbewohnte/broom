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
#include <fstream>
#include <string.h>
#include <cstdint>
#include <vector>
#include <filesystem>

// Broom version number
#define VERSION "v0.1.0"

// Reason why files are considered as duplicates
enum DuplicateBy {
    CHECKSUM,
    FILENAME,
    FILESIZE,
};

// 3 chunks (beginning, end, middle of the file)
const uint8_t CHUNK_SIZE = 24;
const uint8_t CHECKSUM_SIZE = CHUNK_SIZE * 3;

// A wrapper for every file with all necessary information
class Entry {
public:
    Entry(std::filesystem::path path) {
        // check for existense and being a directory
        if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
            throw "Does not exist or a directory";
        };

        // filename
        filename = path.filename();

        // filesize
        filesize = std::filesystem::file_size(path);

        // checksum
        std::fstream entry_file;
        entry_file.open(path);

        if (!entry_file.is_open()) {
            throw "Could not open file";
        }

        char start_buf[CHUNK_SIZE];
        entry_file.read(start_buf, CHUNK_SIZE);

        char end_buf[CHUNK_SIZE];
        entry_file.read(end_buf, CHUNK_SIZE);

        char middle_buf[CHUNK_SIZE];
        entry_file.read(middle_buf, CHUNK_SIZE);

        for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
            if (i < CHUNK_SIZE) {
                checksum[i] = start_buf[i];
            }
            else if (i > CHUNK_SIZE*2) {
                checksum[i] = middle_buf[i-(CHUNK_SIZE*2)];
            }
            else if (i > CHUNK_SIZE) {
                checksum[i] = end_buf[i - CHUNK_SIZE];
            }
        };
    };

    ~Entry() {};

    std::string filename;
    std::filesystem::path path;
    uintmax_t filesize;
    char checksum[CHECKSUM_SIZE];

    // Compare this entry`s checksum with the other one.
    // If the checksums are the same -> returns true, else -> false
    bool compare_checksums(char other_checksum[CHECKSUM_SIZE]) {
        for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
            if (checksum[i] != other_checksum[i]) {
                return false;
            };
        };
        return true;
    };

    // Remove entity from the disk
    void remove() {
        std::filesystem::remove(path);
    };
};

// A class to manage duplicate files
class Broom {
protected:

public:
    Broom() {};
    ~Broom() {};

    // remove ALL duplicate files
    int sweep_all(Entry entries[]) {
        return 0;
    };

    // remove ALL duplicates but the one with specified index
    int sweep_all_but(Entry entries[], uint32_t index = 0) {
        return 0;
    };
};

// A class that wraps in itself ways of locating duplicate files in
// a filesystem
class DupSeeker {
protected:
    // Determines whether entry1 is a duplicate of entry2
    bool is_duplicate(Entry entry1, Entry entry2) {
        if (entry1.path == entry2.path) {
            // well, it`s the same file we`re talking about
            return false;
        } else if (entry1.compare_checksums(entry2.checksum)) {
            // the same checksums. Definitely a duplicate
            return true;
        }else if (entry1.filename == entry2.filename) {
            // probably a duplicate
            return true;
        } else if (entry1.filesize == entry2.filesize) {
            // probably a duplicate
            return true;
        };

        return false;
    };

public:
    DupSeeker() {};
    ~DupSeeker() {};

    // find all duplicates in the directory
    int find_duplicates(std::filesystem::path directory, Entry entries[], bool recursive = false) {
        return 0;
    };
};

// Broom`s settings
struct Options {
    bool sweeping;
    std::vector<std::filesystem::path> paths;
};

void print_help() {
    std::cout
    << "broom [FLAGS..] [COMMAND] [FILES|DIRECTORIES...]" << std::endl << std::endl
    << "FLAGS" << std::endl
    << "-v | --version -> print version information and exit" << std::endl
    << "-h | --help -> print this message and exit" << std::endl << std::endl
    << "COMMANDS" << std::endl
    << "sweep -> scan for duplicate files and delete (sweep) all of them but the last one" << std::endl
    << "scan -> scan for duplicate files and output information in a file" << std::endl
    << std::endl;
};

void print_version() {
    std::cout
    << "broom " << VERSION << std::endl
    << "a command line utility to locate and manage duplicate files" << std::endl << std::endl
    << "Copyright (C) 2021  Kasyanov Nikolay Alexeevich (Unbewohnte (me@unbewohnte.xyz))" << std::endl
    << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
    << "This is free software, and you are welcome to redistribute it" << std::endl
    << "under certain conditions" << std::endl
    << std::endl;
};

int main(int argc, char* argv[]) {
    Options options;

    if (argc < 2) {
        print_help();
        return 0;
    };

    // process command line arguments
    for (unsigned int i = 0; i < argc; i++) {
        // flags -> command -> directories&&files

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
        else if (strcmp(argv[i], "sweep") == 0) {
            options.sweeping = true;
        }
        else if (strcmp(argv[i], "scan") == 0) {
            options.sweeping = false;
        }
        else {
            // add path
            if (i == 0) {
                continue;
            } else {
                options.paths.push_back(argv[i]);
            }
        };
    };

    for (uint32_t i = 0; i < options.paths.size(); i++) {
        std::cout << options.paths.at(i) << std::endl;
    };

    return 0;
};

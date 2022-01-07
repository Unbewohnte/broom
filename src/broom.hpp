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

#ifndef BROOM_HPP
#define BROOM_HPP

#include <cstdint>
#include <vector>

// Broom`s settings
struct Options {
    bool sweeping;
    bool benchmarking;
};


// A class to find and manage duplicate files
class Broom {
protected:
    // enable/disable benchmarking output
    bool m_benchmarking;
    // TODO(think about how sweeping should work)
    bool m_sweeping;

    // how many files has been (would be ?) "sweeped"
    uintmax_t m_sweeped_files;
    // how many bytes was (would be ?) freed
    uintmax_t m_sweeped_size;
    // paths to tracked files
    std::vector<Entry> m_tracked_entries;

public:
    Broom(Options options);
    ~Broom();

    // recursively track every file that lies in given path. Throws an invalid_argument
    // error in case path does not exist
    void track(const std::filesystem::path path);

    // find all duplicates in the directory
    void find_duplicates();

    // removes entries with unique file sizes. Returns amount of files
    // that are no longer being tracked
    uintmax_t untrack_unique_sizes();

    // removes entries with the same content-pieces. Returns amount of
    // files that are no longer being tracked
    uintmax_t untrack_unique_contents();

    // saves current list of duplicate file paths into a file
    void create_duplicates_list(const std::filesystem::path dir = ".", const std::string filename = "duplicate_files_list.txt");

    // TODO
    void sweep_all();
};

#endif

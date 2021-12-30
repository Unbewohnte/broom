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
    bool m_sweeping;

    // how many files has been (would be ?) "sweeped"
    uintmax_t m_sweeped_files;
    // how many bytes was (would be ?) freed
    uintmax_t m_sweeped_size;
    // entries that possibly contain duplicates
    std::vector<Entry> m_tracked_entries;

public:
    Broom(Options options);
    ~Broom();

    // Print current statistics
    void print_statistics();

    // get all entities from path recursively and track them
    void track(const std::filesystem::path path);

    // find all duplicates in the directory
    void find_duplicates();

    // removes entries with unique file sizes. Returns amount of files
    // that are no longer being tracked
    uintmax_t untrack_unique_sizes();

    // removes entries with unique first and last 20 bytes. Returns amount of
    // files that are no longer being tracked
    uintmax_t untrack_unique_contents();

    // remove ALL duplicate files
    void sweep_all();
};

#endif

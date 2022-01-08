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

namespace broom {
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

    // paths to tracked files
    std::vector<entry::Entry> m_tracked_entries;

    // finds empty files among tracked entries.
    // Returns amount of found empty files
    uintmax_t m_find_empty_files();

    // removes entries with unique file sizes. Returns amount of files
    // that are no longer being tracked
    uintmax_t m_untrack_unique_sizes();

    // removes entries with the same content-pieces. Returns amount of
    // files that are no longer being tracked
    uintmax_t m_untrack_unique_contents();

    // finds all duplicates among tracked entries and marks them with appropriate group
    // Returns amount of duplicate files
    uintmax_t m_find_duplicates();

public:
    Broom(Options options);
    ~Broom();

    // recursively track every file that lies in given path. Throws an invalid_argument
    // error in case path does not exist
    void track(const std::filesystem::path path);

    // creates a list of duplicate, empty files into a file
    void create_scan_results_list(const std::filesystem::path dir = ".", const std::string filename = "scan_results.txt");

    // TODO
    void sweep();

    // scans tracked entries for duplicates and empty files
    void scan();
};

}


#endif

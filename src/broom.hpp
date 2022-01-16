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

// A class to find and manage duplicate, empty files
class Broom {
public:
    Broom();
    ~Broom();

    // recursively tracks every file that lies in given path. Throws an invalid_argument
    // error in case path does not exist. Returns collected entries
    std::vector<entry::Entry> track(const std::filesystem::path path);

    // untracks entries with unique file sizes. Returns amount of files
    // that are no longer being tracked
    uintmax_t untrack_unique_sizes(std::vector<entry::Entry>& tracked_entries);

    // untracks entries with the same content-pieces. Returns amount of
    // files that are no longer being tracked.
    uintmax_t untrack_unique_contents(std::vector<entry::Entry>& tracked_entries);

    // finds empty files among tracked entries and marks them with the appropriate group.
    // Returns amount of found empty files
    uintmax_t find_empty_files(std::vector<entry::Entry>& tracked_entries);

    // REMOVES grouped empty files and untracks them after deletion. Returns the amount of removed empty files
    uintmax_t remove_empty_files(std::vector<entry::Entry>& tracked_entries);

    // creates a list of duplicate, empty files into a file
    void create_scan_results_list(const std::vector<entry::Entry> tracked_entries, const std::filesystem::path dir = ".", const std::string filename = "scan_results.txt");
};

}


#endif

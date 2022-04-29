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
#include <map>
#include <string>

#include "entry.hpp"

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
    
    // Untracks specified group in tracked entries. Returns an amount of entries untracked 
    uintmax_t untrack_group(std::vector<entry::Entry>& tracked_entries, entry::Group group);

    // finds empty files among tracked entries and marks them with the appropriate group.
    // Returns amount of found empty files
    uintmax_t find_empty_files(std::vector<entry::Entry>& tracked_entries);

    // REMOVES grouped empty files and untracks them after deletion. Returns the amount of removed empty files
    uintmax_t remove_empty_files(std::vector<entry::Entry>& tracked_entries);

    // marks every entry without any group as a duplicate
    void mark_as_duplicates(std::vector<entry::Entry>& tracked_entries);

    // searches for entries with the same pieces in tracked entries and groups them together as a duplicate group, where the key is the
    // string of pieces. REMOVES EVERYTHING FROM GIVEN TRACKED ENTRIES
    std::map<std::string, std::vector<entry::Entry>> group_duplicates(std::vector<entry::Entry>& tracked_entries);

    // REMOVES every duplicate file in a group except the first one and creates symlinks pointing to the
    // first remaining real file
    void remove_duplicates_make_symlinks(const std::map<std::string, std::vector<entry::Entry>> grouped_duplicates);

    // creates a list of duplicate, empty files and puts it into a file
    void create_scan_results_list(const std::map<std::string, std::vector<entry::Entry>> grouped_duplicates, const std::filesystem::path dir = ".", const std::string filename = "scan_results.txt");
};

}


#endif

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
#include <algorithm>
#include <map>

#include "entry.hpp"
#include "broom.hpp"

Broom::Broom() {};
Broom::~Broom() {};

// Print current statistics
void Broom::print_statistics() {
    std::cout
    << "| sweeped " << m_sweeped_files << " files" << std::endl
    << "| with a total size of " << m_sweeped_size << " bytes" << std::endl
    << std::endl;
};

// removes entries with unique file sizes. Returns amount of files
// that are no longer being tracked
uintmax_t Broom::untrack_unique_sizes() {
    // key: size, value: amount of occurences
    std::map<uintmax_t, uintmax_t> sizes;
    std::map<uintmax_t, uintmax_t>::iterator iterator;

    for (Entry& entry : m_tracked_entries) {
        // check if size of this entry is already in the map
        // if yes --> increment occurences counter
        // if not --> add it to the map with a counter of 1
        iterator = sizes.find(entry.filesize);
        if (iterator == sizes.end()) {
            // there is no such size
            sizes.insert(std::pair<uintmax_t, uintmax_t>(entry.filesize, 1));
        } else {
            // there is such size
            uintmax_t occurences = sizes[iterator->first];
            sizes[iterator->first] = occurences++;
        };
    };

    // go through the map again, look for uniques and remove entries with
    // such filesizes
    uintmax_t untracked = 0;
    for (std::pair<uintmax_t, uintmax_t> size_entry : sizes) {
        if (size_entry.second > 1) {
            // not a unique size. Keep such entries
        } else {
            // a unique one. Untrack such an entry
            std::remove_if(m_tracked_entries.begin(), m_tracked_entries.end(), [size_entry](Entry e) -> bool {
                return (e.filesize == size_entry.first);
            });
            untracked++;

        };
    };

    return untracked;
};

// get all entities from path recursively and track them
int Broom::track(std::filesystem::path dir) {
    std::filesystem::directory_options options =  (
        std::filesystem::directory_options::follow_directory_symlink |
        std::filesystem::directory_options::skip_permission_denied
    );

    for (std::filesystem::directory_entry dir_entry : std::filesystem::recursive_directory_iterator(dir, options)) {
        if (dir_entry.is_directory()) {
            continue;
        };

        Entry entry(dir_entry.path());
        m_tracked_entries.push_back(entry);
    };

    return 0;
};

// find all duplicates among tracked entries
int Broom::find_duplicates() {
    size_t startsize = m_tracked_entries.size();
    std::cout << "Tracking " << startsize << std::endl;

    uintmax_t untracked = untrack_unique_sizes();
    std::cout << "Untracked " << untracked << " unique sizes" << std::endl;

    std::cout << "Duplicates: " << startsize - untracked << std::endl;
    return 0;
};

// remove ALL duplicate files
int Broom::sweep_all(Entry entries[]) {
    return 0;
};

// remove ALL duplicates but the one with specified index
int Broom::sweep_all_but(Entry entries[], uint32_t index = 0) {
    return 0;
};

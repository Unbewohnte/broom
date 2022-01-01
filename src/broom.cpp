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
#include <chrono>

#include "entry.hpp"
#include "broom.hpp"

Broom::Broom(Options options) {
    m_benchmarking = options.benchmarking;
    m_sweeping = options.sweeping;
};

Broom::~Broom() {};

// Print current statistics
void Broom::print_statistics() {
    std::cout
    << "| sweeped " << m_sweeped_files << " files" << std::endl
    << "| with a total size of " << m_sweeped_size << " bytes" << std::endl
    << std::endl;
};

// get all files from path recursively and track them
void Broom::track(const std::filesystem::path dir) {
    auto t0 = std::chrono::high_resolution_clock::now();

    std::filesystem::directory_options options =  (
        std::filesystem::directory_options::follow_directory_symlink |
        std::filesystem::directory_options::skip_permission_denied
    );

    for (std::filesystem::directory_entry dir_entry : std::filesystem::recursive_directory_iterator(dir, options)) {
        if (dir_entry.is_directory()) {
            continue;
        };

        m_tracked_filepaths.push_back(dir_entry);
    };

    if (m_benchmarking) {
        auto tracking_time = std::chrono::high_resolution_clock::now();

        std::cout
        << "Tracking took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(tracking_time - t0).count()
        << " ms" << std::endl;
    };
};

// removes entries with unique file sizes. Returns amount of files
// that are no longer being tracked
uintmax_t Broom::untrack_unique_sizes() {
    // key: size, value: amount of occurences
    std::map<uintmax_t, uintmax_t> sizes_map;

    for (std::filesystem::path filepath : m_tracked_filepaths) {
        // check if size of this entry is already in the map
        // if yes --> increment occurences counter
        // if not --> add it to the map with a counter of 1
        uintmax_t filesize = std::filesystem::file_size(filepath);
        auto iterator = sizes_map.find(filesize);
        if (iterator == sizes_map.end()) {
            // there is no such size
            sizes_map.insert({filesize, 1});
        } else {
            // there is such size
            sizes_map[iterator->first]++;
        };
    };

    uintmax_t untracked = 0;
    m_tracked_filepaths.erase(std::remove_if(m_tracked_filepaths.begin(), m_tracked_filepaths.end(), [&untracked, sizes_map](std::filesystem::path filepath) -> bool{
        uintmax_t filesize = std::filesystem::file_size(filepath);
        auto iter = sizes_map.find(filesize);
        if (iter->second == 1) {
            // unique
            untracked++;
            return true;
        };

        // std::cout << "duplicate fsize: " << iter->first << " occurences: " << iter->second << std::endl;

        return false;
    }));


    return untracked;
};

// removes entries with unique first and last 20 bytes. Returns amount of
// files that are no longer being tracked
// uintmax_t Broom::untrack_unique_contents() {
//     // contents, occurences
//     std::map<char[CHECKSUM_SIZE], uintmax_t> contents_map;
//     std::map<char[CHECKSUM_SIZE], uintmax_t>::iterator iterator;
//
//     for (Entry& entry : m_tracked_filepaths) {
//         // the same logic:
//         // check if contents of this entry is already in the map
//         // if yes --> increment occurences counter
//         // if not --> add it to the map with a counter of 1
//
//         iterator = contents_map.find(entry.checksum);
//
//         if (iterator == contents_map.end()) {
//             // add it to the map
//             contents_map.insert(std::pair<char[CHECKSUM_SIZE], uintmax_t>(entry.checksum, 1));
//         } else {
//             // increment occurences counter
//             uintmax_t occurences = contents_map[iterator->first];
//             contents_map[iterator->first] = occurences++;
//         };
//     };
//
//     uintmax_t untracked = 0;
//     for (std::pair<const char[CHECKSUM_SIZE], uintmax_t> contents_entry : contents_map) {
//         if (contents_entry.second > 1) {
//             // not a unique size. Keep such entries
//         } else {
//             // a unique one. Untrack such an entry
//             std::remove_if(m_tracked_filepaths.begin(), m_tracked_filepaths.end(), [contents_entry](Entry e) -> bool {
//                 return (e.compare_checksums(contents_entry.first));
//             });
//             untracked++;
//         };
//     };
//
//     return untracked;
// };


// find all duplicates among tracked entries, stop tracking uniques
void Broom::find_duplicates() {
    if (m_benchmarking) {
        auto t0 = std::chrono::high_resolution_clock::now();

        untrack_unique_sizes();

        auto sizes_untrack_time = std::chrono::high_resolution_clock::now();

        std::cout
        << "Untracking by size took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(sizes_untrack_time - t0).count()
        << " ms" << std::endl

        << std::endl;
    } else {
        size_t startsize = m_tracked_filepaths.size();
        std::cout << "Tracking " << startsize << std::endl;

        uintmax_t global_untracked = 0;

        // uintmax_t untracked_by_contents = untrack_unique_contents();
        // global_untracked += untracked_by_contents;
        // std::cout << "Untracked " << untracked_by_contents << " unique contents" << std::endl;


        uintmax_t untracked_by_size = untrack_unique_sizes();
        global_untracked += untracked_by_size;
        std::cout << "Untracked " << untracked_by_size << " unique sizes" << std::endl;


        std::cout << "Duplicates: " << startsize - global_untracked << std::endl;
    };
};

// remove ALL duplicate files
void Broom::sweep_all() {
};

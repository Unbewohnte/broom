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
#include <iterator>
#include <map>
#include <chrono>
#include <stdexcept>

#include "entry.hpp"
#include "broom.hpp"
#include "group.hpp"

namespace broom {

Broom::Broom(Options options) {
    m_benchmarking = options.benchmarking;
    m_sweeping = options.sweeping;
};

Broom::~Broom() {
    m_tracked_entries.clear();
};

// recursively track every file that lies in given path. Throws an invalid_argument
// error in case path does not exist
void Broom::track(const std::filesystem::path path) {
    auto t0 = std::chrono::high_resolution_clock::now();

    // check if given path even exists
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument("\"" + path.string() + "\"" + " does not exist !");
    }

    if (std::filesystem::is_directory(path)) {
        // it`s a directory. Track every regular file recursively
        std::filesystem::directory_options options =  (
        std::filesystem::directory_options::skip_permission_denied
        );

        for (auto dir_entry : std::filesystem::recursive_directory_iterator(path, options)) {
            if (!dir_entry.is_regular_file()) {
                // skip everything that we cannot process so easily
                continue;
            };

            entry::Entry entry(dir_entry.path());
            m_tracked_entries.push_back(entry);
        }
    } else if (std::filesystem::is_regular_file(path)) {
        // just a file
        entry::Entry entry(path);
        m_tracked_entries.push_back(entry);
    }


    if (m_benchmarking) {
        auto tracking_time = std::chrono::high_resolution_clock::now();

        std::cout
        << "[BENCHMARK] Tracking took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(tracking_time - t0).count()
        << " ms" << std::endl;
    }

    std::cout << "[INFO] Tracking " << m_tracked_entries.size() << " files" << std::endl;
};

// removes entries with unique file sizes. Returns amount of files
// that are no longer being tracked
uintmax_t Broom::m_untrack_unique_sizes() {
    // key: size, value: amount of occurences
    std::map<uintmax_t, uintmax_t> sizes_map;

    for (auto entry_iter = m_tracked_entries.begin(); entry_iter != m_tracked_entries.end(); entry_iter++) {
        // check if size of this entry is already in the map
        // if yes --> increment occurences counter
        // if not --> add it to the map with a counter of 1
        auto iterator = sizes_map.find(entry_iter->filesize);
        if (iterator == sizes_map.end()) {
            // there is no such size
            sizes_map.insert({entry_iter->filesize, 1});
        } else {
            // there is such size
            sizes_map[iterator->first]++;
        }
    }

    uintmax_t untracked = 0;
    m_tracked_entries.erase(std::remove_if(m_tracked_entries.begin(), m_tracked_entries.end(), [&untracked, sizes_map](entry::Entry entry) -> bool{
        auto iter = sizes_map.find(entry.filesize);
        if (iter->second == 1) {
            // unique
            untracked++;
            return true;
        };

        return false;
    }), m_tracked_entries.end());

    return untracked;
};

// removes entries with the same content-pieces. Returns amount of
// files that are no longer being tracked
uintmax_t Broom::m_untrack_unique_contents() {
    // contents, occurences
    std::map<std::string, uintmax_t> contents_map;
    std::map<std::string, uintmax_t>::iterator map_iter;

    for (auto entry_iter = m_tracked_entries.begin(); entry_iter != m_tracked_entries.end();) {
        // the same logic:
        // check if contents of this entry are already in the map
        // if yes --> increment occurences counter
        // if not --> add it to the map with a counter of 1

        if (entry_iter->filesize == 0) {
            // that`s an empty file. Skip it
            entry_iter++;
            continue;
        }

        try{
            // can get "permission denied" when opening file
            entry_iter->get_pieces();
        } catch(const std::ifstream::failure& e) {
            // there is nothing we can do. Untrack this entry
            entry_iter = m_tracked_entries.erase(entry_iter);
            continue;
        }

        map_iter = contents_map.find(entry_iter->pieces);
        if (map_iter == contents_map.end()) {
            // add it to the map
            contents_map.insert({entry_iter->pieces, 1});
            // std::cout << "First time seeing this piece: " << entry_iter->pieces << std::endl;
        } else {
            // increment occurences counter
            contents_map[map_iter->first]++;
        }

        entry_iter++;
    };

    uintmax_t untracked = 0;
    m_tracked_entries.erase(std::remove_if(m_tracked_entries.begin(), m_tracked_entries.end(), [&untracked, contents_map](entry::Entry entry) -> bool {
        auto iter = contents_map.find(entry.pieces);
        if (iter->second == 1) {
            // unique
            untracked++;
            return true;
        } else {
            return false;
        }
    }), m_tracked_entries.end());

    return untracked;
};

// finds all duplicates among tracked entries and marks them with appropriate group.
// Returns amount of duplicate files
uintmax_t Broom::m_find_duplicates() {
    auto t0 = std::chrono::high_resolution_clock::now();

    // print how many files are being tracked
    uintmax_t global_untracked = m_tracked_entries.size();

    // untrack by size
    uintmax_t untracked_by_size = m_untrack_unique_sizes();
    global_untracked += untracked_by_size;
    std::cout << "[INFO] Untracked " << untracked_by_size << " unique sizes" << std::endl;

    auto sizes_untrack_time = std::chrono::high_resolution_clock::now();

    if (m_benchmarking) {
        std::cout
        << "[BENCHMARK] Untracking by size took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(sizes_untrack_time - t0).count()
        << " ms" << std::endl;
    }

    // untrack by contents
    uintmax_t untracked_by_contents = m_untrack_unique_contents();
    global_untracked += untracked_by_contents;

    auto contents_untrack_time = std::chrono::high_resolution_clock::now();

    if (m_benchmarking) {
        std::cout
        << "[BENCHMARK] Untracking by contents took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(contents_untrack_time - sizes_untrack_time).count()
        << " ms" << std::endl;
    }

    std::cout << "[INFO] Untracked " << untracked_by_contents << " unique contents" << std::endl;

    std::cout << "[INFO] Found " << m_tracked_entries.size() << " possible duplicate files" << std::endl;

    // mark duplicate entries

    for (entry::Entry& duplicate_entry : m_tracked_entries) {
        duplicate_entry.group = group::DUPLICATE;
    }

    return m_tracked_entries.size();
};

// creates a list of duplicate, empty files into a file
void Broom::create_scan_results_list(const std::filesystem::path dir, const std::string filename) {
    if (!std::filesystem::exists(dir)) {
        // create it then
        bool created = std::filesystem::create_directories(dir);
        if (!created) {
            throw "Could not create a directory to save scan results in";
        }
    }

    // create output file there
    std::fstream outfile(dir / filename, std::ios::out);
    if (!outfile.is_open()) {
        throw "Could not create a scan results file";
    }

    for (const entry::Entry entry : m_tracked_entries) {
        // log every entry and its group
        if (entry.group == group::EMPTY) {
            outfile << entry.path << " --- is an empty file" << std::endl;
        } else if (entry.group == group::DUPLICATE) {
            outfile << entry.path << " --- is a duplicate of another file" << std::endl;
        }
    }

    outfile.close();

    std::cout << "[INFO] Created scan results file" << std::endl;
};

// finds empty files among tracked entries and gives them appropriate group
// Returns amount of found empty files
uintmax_t Broom::m_find_empty_files() {
    uintmax_t found_empty_files = 0;
    for (entry::Entry& entry : m_tracked_entries) {
        if (entry.filesize == 0) {
            entry.group = group::EMPTY;
            found_empty_files++;
        }
    }

    std::cout << "[INFO] Found " << found_empty_files << " empty files" << std::endl;

    return found_empty_files;
};

// scans directory for duplicates and empty files
void Broom::scan() {
    m_find_empty_files();
    m_find_duplicates();
};

// remove ALL duplicate files
void Broom::sweep() {
};


}

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
#include <future>

#include "entry.hpp"
#include "broom.hpp"
#include "group.hpp"

namespace broom {

Broom::Broom() {};
Broom::~Broom() {};

// recursively track every file that lies in given path. Throws an invalid_argument
// error in case path does not exist
std::vector<entry::Entry> Broom::track(const std::filesystem::path path) {
    std::vector<entry::Entry> tracked_entries;

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
            tracked_entries.push_back(entry);
        }
    } else if (std::filesystem::is_regular_file(path)) {
        // just a file
        entry::Entry entry(path);
        tracked_entries.push_back(entry);
    }

    return tracked_entries;
};

// untracks entries with unique file sizes. Returns amount of files
// that are no longer being tracked
uintmax_t Broom::untrack_unique_sizes(std::vector<entry::Entry>& tracked_entries) {
    // key: size, value: amount of occurrences
    std::map<uintmax_t, uintmax_t> sizes_map;

    for (auto entry_iter = tracked_entries.begin(); entry_iter != tracked_entries.end(); entry_iter++) {
        // check if size of this entry is already in the map
        // if yes --> increment occurrences counter
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
    tracked_entries.erase(std::remove_if(tracked_entries.begin(), tracked_entries.end(), [&untracked, sizes_map](entry::Entry entry) -> bool{
        auto iter = sizes_map.find(entry.filesize);
        if (iter->second == 1) {
            // unique
            untracked++;
            return true;
        };

        return false;
    }), tracked_entries.end());

    return untracked;
};


// untracks entries with the same content-pieces. Returns amount of
// files that are no longer being tracked
uintmax_t Broom::untrack_unique_contents(std::vector<entry::Entry>& tracked_entries) {
    // contents, occurrences
    std::map<std::string, uintmax_t> contents_map;
    std::map<std::string, uintmax_t>::iterator map_iter;

    for (entry::Entry& entry : tracked_entries) {
        // the same logic:
        // check if contents of this entry are already in the map
        // if yes --> increment occurrences counter
        // if not --> add it to the map with a counter of 1
        map_iter = contents_map.find(entry.pieces);
        if (map_iter == contents_map.end()) {
            // add it to the map
            contents_map.insert({entry.pieces, 1});
        } else {
            // increment occurrences counter
            contents_map[map_iter->first]++;
        }

    };

    uintmax_t untracked = 0;
    tracked_entries.erase(std::remove_if(tracked_entries.begin(), tracked_entries.end(), [&untracked, contents_map](entry::Entry entry) -> bool {
        auto iter = contents_map.find(entry.pieces);
        if (iter->second == 1) {
            // unique
            untracked++;
            return true;
        } else {
            return false;
        }
    }), tracked_entries.end());

    return untracked;
};

// creates a list of duplicate, empty files and puts it into a file
void Broom::create_scan_results_list(const std::map<std::string, std::vector<entry::Entry>> grouped_duplicates, const std::filesystem::path dir, const std::string filename) {
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

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    outfile << ">> Broom scan results file from " << std::ctime(&now) << std::endl << std::endl << std::endl;

    for (const auto record : grouped_duplicates) {
        if (record.first == "") {
            outfile << "[EMPTY FILES]" << std::endl;
        } else {
            outfile << "[DUPLICATE GROUP]" << std::endl;
        }

        for (const auto duplicate_entry : record.second) {
            outfile << duplicate_entry.path << std::endl;
        }

        outfile << std::endl << std::endl;
    }

    outfile.close();
};

// finds empty files among tracked entries and gives them appropriate group
// Returns amount of found empty files
uintmax_t Broom::find_empty_files(std::vector<entry::Entry>& tracked_entries) {
    uintmax_t found_empty_files = 0;
    for (entry::Entry& entry : tracked_entries) {
        if (entry.filesize == 0) {
            // empty files can`t be considered as duplicates. assign a group
            entry.group = group::EMPTY;
            found_empty_files++;
        }
    }

    return found_empty_files;
};

// REMOVES grouped empty files and untracks them after deletion. Returns the amount of removed empty files
uintmax_t Broom::remove_empty_files(std::vector<entry::Entry>& tracked_entries) {
    uintmax_t removed = 0;

    tracked_entries.erase(std::remove_if(tracked_entries.begin(), tracked_entries.end(), [&removed](entry::Entry& entry) -> bool {
        if (entry.group == group::EMPTY) {
            try {
                entry.remove();
                removed++;
                return true;
            } catch(...) {
                return true;
            }

        }
        return false;

    }), tracked_entries.end());

    return removed;
};

// marks every entry without any group as a duplicate
void Broom::mark_as_duplicates(std::vector<entry::Entry>& tracked_entries) {
    for (entry::Entry& entry : tracked_entries) {
        if (entry.group == group::EMPTY) {
            // do not mess up grouping
            continue;
        }
        entry.group = group::DUPLICATE;
    }
};


// searches for entries with the same pieces in tracked entries and groups them together as a duplicate group, where the key is the
// string of pieces. REMOVES EVERYTHING FROM GIVEN TRACKED ENTRIES
std::map<std::string, std::vector<entry::Entry>> Broom::group_duplicates(std::vector<entry::Entry>& tracked_entries) {
    std::map<std::string, std::vector<entry::Entry>> duplicate_groups;

    for (auto iter = tracked_entries.begin(); iter != tracked_entries.end(); iter++) {
      auto map_iter = duplicate_groups.find(iter->pieces);
      if (map_iter == duplicate_groups.end()) {
        // first time seeing these pieces
        std::vector<entry::Entry> occurences;
        occurences.push_back(*iter);
        duplicate_groups.insert({iter->pieces, occurences});
      } else {
        // add to occurrences this entry
        duplicate_groups[map_iter->first].push_back(*iter);
      }
    };

    // clear the vector
    tracked_entries.clear();

    return duplicate_groups;
};

}

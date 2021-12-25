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

// Determines whether entry1 is a duplicate of entry2
bool Broom::is_duplicate(Entry entry1, Entry entry2) {
    if (entry1.path == entry2.path) {
        // well, it`s the same file we`re talking about
        return false;
    }
    else if (entry1.compare_checksums(entry2.checksum)) {
        return true;
    }

    return false;
};

// find all duplicates in the directory
int Broom::find_duplicates(std::filesystem::path directory, Entry entries[], bool recursive = false) {
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

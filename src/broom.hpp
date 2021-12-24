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

# ifndef BROOM_HPP
# define BROOM_HPP

#include <cstdint>
#include <iostream>

// A class to find and manage duplicate files
class Broom {
protected:
    // how many files has been "sweeped"
    uintmax_t m_sweeped_files;
    // how many bytes was freed
    uintmax_t m_sweeped_size;

public:
    Broom() {};
    ~Broom() {};

    // Print current statistics
    void print_statistics();

    // Determines whether entry1 is a duplicate of entry2
    bool is_duplicate(Entry entry1, Entry entry2);

    // find all duplicates in the directory
    int find_duplicates(std::filesystem::path directory, Entry entries[], bool recursive = false);

    // remove ALL duplicate files
    int sweep_all(Entry entries[]);

    // remove ALL duplicates but the one with specified index
    int sweep_all_but(Entry entries[], uint32_t index = 0);
};

# endif

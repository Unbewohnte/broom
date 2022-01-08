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

#include "entry.hpp"

#include <iostream>
#include <cerrno>
#include <cstring>


// A wrapper for every file with all necessary information
Entry::Entry(const std::filesystem::path entry_path) {
    // path
    path = entry_path;
};

Entry::~Entry() {};

// sets this entry`s filesize
void Entry::get_size() {
    filesize = std::filesystem::file_size(path);
};


// reads 2 pieces from the middle and the end of a file, converts them into
// a convenient hex-encoded string
void Entry::get_pieces() {
    std::fstream entry_file;
    entry_file.open(path);

    if (!entry_file.is_open()) {
        throw std::ifstream::failure("Could not open \"" + path.string() + "\"; reason: " + std::string(std::strerror(errno)) + "\n");
    }

    char pieces_buffer[PIECE_SIZE * 2];
    if (filesize <= PIECE_SIZE * 2) {
        // can`t take whole 2 pieces !
        // read the whole file then
        entry_file.read(pieces_buffer, filesize);
    } else {
        uintmax_t middle_of_the_file = (double) filesize / 2.0 - PIECE_SIZE;

        entry_file.seekg(middle_of_the_file, std::ios::beg);
        // read CHUNK_SIZE bytes from the middle of the file
        char middle_buf[PIECE_SIZE];
        entry_file.read(middle_buf, PIECE_SIZE);
        for (uint8_t i = 0; i < PIECE_SIZE; i++) {
            pieces_buffer[i] = middle_buf[i];
        };

        // jump to the last CHUNK_SIZE bytes of the file and read the as well
        entry_file.seekg(PIECE_SIZE, std::ios::end);
        char end_buf[PIECE_SIZE];
        entry_file.read(end_buf, PIECE_SIZE);
        for (uint8_t i = PIECE_SIZE; i < PIECE_SIZE * 2; i++) {
            pieces_buffer[i] = end_buf[i - PIECE_SIZE];
        };
    };
    entry_file.close();

    // make a convenient hex string out of pure bytes
    std::stringstream pieces_hex;
    for (uint8_t i = 0; i < PIECE_SIZE * 2; i++) {
        pieces_hex << std::hex << static_cast<unsigned>(pieces_buffer[i]);
    };

    pieces = pieces_hex.str();

    std::cout << pieces << std::endl;
};

// Remove entry from the disk
void Entry::remove() {
    std::filesystem::remove(path);
};

//
// Created by ruslan on 3/3/24.
//

#include "file-reverse.h"

#include <cstring>
#include <fstream>
#include <iostream>

enum Encoding {
    ASCII,
    UTF8,
    UTF16,
    UTF32,
    UNKNOWN,
};

Encoding detect_bom(std::fstream &file) {
    char buf[4];
    file.read(buf, 4);
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0xFE && buf[3] == 0xFF) // utf32be
        return UTF32;
    if(buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0 && buf[3] == 0) // utf32le
        return UTF32;
    if(buf[0] == 0xFE && buf[1] == 0xFF) // utf16be
        return UTF16;
    if(buf[0] == 0xFF && buf[1] == 0xFE) // utf16le
        return UTF16;
    if(buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) // utf8
        return UTF8;

    return UNKNOWN;
}

Encoding detect_utf32(std::fstream &file) {
    file.clear();
    file.seekg(0, std::ios::beg);
    char buf[2];
    while(file.good()) {
        file.read(buf, 2);
        if(buf[0] == 0 && buf[1] == 0)
            return UTF32;
    }
    return UNKNOWN;
}

Encoding detect_utf16(std::fstream &file) {
    file.clear();
    file.seekg(0, std::ios::beg);
    char buf[1];
    while(file.good()) {
        file.read(buf, 1);
        if(buf[0] == 0)
            return UTF16;
    }
    return UNKNOWN;
}

Encoding detect_utf8(std::fstream &file) {
    file.clear();
    file.seekg(0, std::ios::beg);
    char buf[1];
    while(file.good()) {
        file.read(buf, 1);
        if((buf[0] & 0b10000000) == 0b10000000)
            return UTF8;
    }
    return UNKNOWN;
}

Encoding detect_encoding(std::fstream &file) {
    Encoding enc;
    if((enc = detect_bom(file)) != UNKNOWN)
        return enc;

    file.clear();
    file.seekg(0, std::ios::end);
    long file_size = file.tellg();

    if(file_size % 4 == 0)
        if((enc = detect_utf32(file)) != UNKNOWN)
            return enc;

    if(file_size % 2 == 0)
        if((enc = detect_utf16(file)) != UNKNOWN)
            return enc;

    if((enc = detect_utf8(file)) != UNKNOWN)
        return enc;

    return ASCII;
}

void _reverse(std::fstream &file, char* path, int char_size) {
    std::fstream outFile(std::string(path)+".reversed.txt", std::ios::out | std::ios::binary | std::ios::trunc);

    file.clear();
    file.seekg(-char_size, std::ios::end);
    long size = file.tellg() + (long)char_size;

    char buf[char_size];
    for(int i = 0; i < size; i += char_size) {
        file.read(buf, char_size);
        outFile.write(buf, char_size);
        file.seekg(-char_size*2, std::ios::cur);
    }

    outFile.close();
}

void reverse_utf8(std::fstream &file, char* path) {
    std::fstream outFile(std::string(path)+".reversed.txt", std::ios::out | std::ios::binary | std::ios::trunc);

    file.clear();
    file.seekg(-1, std::ios::end);
    long size = file.tellg() + 1L;

    char buf[4];
    int i = 0;
    while(i < size) {
        int bufsize = 1;
        file.read(buf, 1);

        while((buf[bufsize-1] & 0b11000000) == 0b10000000 && bufsize < 4) {
            file.seekg(-2, std::ios::cur);
            file.read(buf+bufsize, 1);
            bufsize++;
        }

        for (int j = 0; j < bufsize / 2; j++) {
            std::swap(buf[j], buf[bufsize - j - 1]);
        }

        outFile.write(buf, bufsize);
        file.seekg(-2, std::ios::cur);
        i += bufsize;
    }

    outFile.close();
}

void f_reverse(char* path) {
    std::fstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        printf("Failed to open file.\n");
        if (file.fail())
            printf("Reason: %s\n", strerror(errno));
        return;
    }

    Encoding enc = detect_encoding(file);
    switch (enc) {
        default:
        case ASCII: {
            _reverse(file, path, 1);
            break;
        }
        case UTF8: {
            reverse_utf8(file, path);
            break;
        }
        case UTF16: {
            _reverse(file, path, 2);
            break;
        }
        case UTF32: {
            _reverse(file, path, 4);
            break;
        }
    }

    printf("12. File %s reversed\n", path);

    file.close();
}

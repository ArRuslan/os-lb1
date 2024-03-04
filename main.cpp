#include <algorithm>
#include <cstring>
#include <iostream>

#include "file-reverse.h"

#ifdef UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif

void reop_stdout() {
    freopen(nullptr, "a", stdout);
}

/*void hex(const char *str) {
    for (int i = 0; i < strlen(str); ++i)
        printf("%02X ", str[i]);
}

void hex(const wchar_t *wstr, const size_t size) {
    for (int i = 0; i < size; ++i)
        wprintf(L"%04X ", static_cast<int>(wstr[i]));
}*/

// ReSharper disable once CppDFAConstantConditions
void task2() {
    ssize_t char_size = sizeof(TCHAR);
    if (char_size == 1)
        printf("2. Char size is 1 byte, using ASCII.\n");
    else
        printf("2. Char size is %llu bytes, probably using UNICODE.\n", char_size);
}

void task3() {
#ifdef UNICODE
    printf("3. Using unicode because UNICODE is defined.\n");
#else
    printf("3. Using ascii because UNICODE is not defined.\n");
#endif
}

void task6(char *strs[], const int length) {
    printf("6. Multibyte: \n");
    for (int i = 0; i < length; i++)
        printf("    %s\n", strs[i]);
}

int task_9_compare(const void* a, const void* b) {
    return wcscmp(*(wchar_t**)a, *(wchar_t**)b);
}

void task7_8_9_10_11(char *strs[], const int length) {
    setlocale(LC_ALL, "uk_UA.utf8");

    reop_stdout();
    wprintf(L"8. Wide-char: \n");

    wchar_t *wchar_strs[length];

    for (int i = 0; i < length; i++) {
        const size_t buf_size = mbstowcs(nullptr, strs[i], 0);
        wchar_strs[i] = new wchar_t[buf_size + 1];
        mbstowcs(wchar_strs[i], strs[i], buf_size + 1);

        wprintf(L"    (wprintf): %ls\n", wchar_strs[i]);
        std::wcout << L"    (wcout)  : " << wchar_strs[i] << "\n\n";
    }

    // Task 9
    qsort(wchar_strs, length, sizeof(wchar_t*), task_9_compare);
    std::sort(wchar_strs, wchar_strs+length, [=](const wchar_t* a, const wchar_t* b) {
        return wcscmp(a, b) < 0;
    });

    // Task 10
    for (int i = 0; i < length; i++) {
        const size_t buf_size = wcstombs(nullptr, wchar_strs[i], 0);
        strs[i] = new char[buf_size+1];
        wcstombs(strs[i], wchar_strs[i], buf_size+1);
    }

    // Task 11
    reop_stdout();
    for (int i = 0; i < length; i++)
        printf("    (back to char): %s\n", strs[i]);
}

void task_12() {
    f_reverse("files/ascii.txt");
    f_reverse("files/utf8.txt");
    f_reverse("files/utf16be.txt");
    f_reverse("files/utf16le.txt");
    f_reverse("files/utf32be.txt");
    f_reverse("files/utf32le.txt");
}

int main() {
    task2();
    task3();

    setlocale(LC_ALL, "uk_UA.utf8");

    char *str[] = {"ПІБ 1", "aПІБ 2", "bПІБ 3"};
    task6(str, 3);
    task7_8_9_10_11(str, 3);

    task_12();

    return 0;
}

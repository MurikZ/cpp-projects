#include "word.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ranges>

using namespace std;

// Объявление функции ДО ее использования
size_t count_utf8_chars(const string& str);

Stats analyzeFile(const string& filename) {
    Stats stats;
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Ошибка при открытии файла: " << filename << endl;
        return stats;
    }

    // Получаем размер файла
    file.seekg(0, ios::end);
    stats.bytes = file.tellg();
    file.seekg(0);

    // Устанавливаем локаль для корректной работы с UTF-8
    setlocale(LC_ALL, "en_US.utf8");
    ios_base::sync_with_stdio(false);
    locale utf8_locale("en_US.UTF-8");
    file.imbue(utf8_locale);

    string line;
    while (getline(file, line)) {
        stats.lines++;
        stats.chars += count_utf8_chars(line) + 1; // +1 для \n
        stringstream ss(line);
        string word;
        while (ss >> word) {
            stats.words++;
        }
    }

    file.close();
    return stats;
}


size_t count_utf8_chars(const string& str) {
    return ranges::count_if(str, [](char c) {
        return (static_cast<unsigned char>(c) & 0xC0) != 0x80;
    });
}

void printStats(const Stats& stats, const string& filename, const set<char>& options) {
    if (options.count('l')) cout << stats.lines << " ";
    if (options.count('w')) cout << stats.words << " ";
    if (options.count('c')) cout << stats.bytes << " ";
    if (options.count('m')) cout << stats.chars << " ";
    cout << filename << endl;
}
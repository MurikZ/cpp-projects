#include "word.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>

using namespace std;

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

    // Проверяем, есть ли \n в конце файла
    bool has_newline = false;
    if (stats.bytes > 0) {
        file.seekg(-1, ios::end);
        char last_char;
        file.get(last_char);
        has_newline = (last_char == '\n');
        file.seekg(0);
    }

    // Устанавливаем локаль
    setlocale(LC_ALL, "en_US.utf8");
    locale utf8_locale("en_US.UTF-8");
    file.imbue(utf8_locale);

    string line;
    while (getline(file, line)) {
        stats.lines++;
        stats.chars += count_utf8_chars(line);
        
        // Добавляем +1 только если:
        // 1) Это не последняя строка ИЛИ
        // 2) В файле есть \n в конце
        if (!file.eof() || has_newline) {
            stats.chars++;
        }

        // Подсчёт слов
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
    size_t count = 0;
    for (char c : str) {
        if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

void printStats(const Stats& stats, const string& filename, const set<char>& options) {
    if (options.count('l')) cout << stats.lines << " ";
    if (options.count('w')) cout << stats.words << " ";
    if (options.count('c')) cout << stats.bytes << " ";
    if (options.count('m')) cout << stats.chars << " ";
    cout << filename << endl;
}
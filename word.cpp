#include "word.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Stats analyzeFile(const string& filename) {
    Stats stats;
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Ошибка при открытии файла: " << filename << endl;
        return stats;
    }

    file.seekg(0, ios::end);
    stats.bytes = file.tellg();
    file.seekg(0);

    string line;
    while (getline(file, line)) {
        stats.lines++;
        stats.chars += line.size() + 1;
        stringstream ss(line);
        string word;
        while (ss >> word) stats.words++;
    }

    file.close();
    return stats;
}

void printStats(const Stats& stats, const string& filename, const set<char>& options) {
    if (options.count('l')) cout << stats.lines << " ";
    if (options.count('w')) cout << stats.words << " ";
    if (options.count('c')) cout << stats.bytes << " ";
    if (options.count('m')) cout << stats.chars << " ";
    cout << filename << endl;
}

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <limits>
#include <stdexcept>
#include "word.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        set<char> options;
        vector<string> filenames;

        // Разбор аргументов
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg.empty() || arg[0] == '\0') continue;  // Пропускаем пустые аргументы
            
            if (arg[0] == '-') {
                if (arg.substr(0, 2) == "--") {
                    if (arg == "--lines") options.insert('l');
                    else if (arg == "--words") options.insert('w');
                    else if (arg == "--bytes") options.insert('c');
                    else if (arg == "--chars") options.insert('m');
                } else {
                    for (size_t j = 1; j < arg.size(); ++j)
                        options.insert(arg[j]);
                }
            } else {
                filenames.push_back(arg);
            }
        }

        if (options.empty()) {
            options = {'l', 'w', 'c'}; // Стандартные флаги
        }

        // Если файлы не указаны, запросить ввод
        if (filenames.empty()) {
            cout << "Введите имя файла: ";
            string filename;
            getline(cin, filename);  // Читаем всю строку
            
            // Проверка на пустую строку
            if (filename.empty() || filename[0] == '\0') {
                cerr << "Ошибка: введено пустое имя файла" << endl;
                return 1;
            }
            
            filenames.push_back(filename);
        }

        // Обработка файлов
        for (const string& filename : filenames) {
            // Проверка на пустое имя файла
            if (filename.empty() || filename[0] == '\0') {
                cerr << "Ошибка: обнаружено пустое имя файла" << endl;
                continue;
            }
            
            try {
                Stats stats = analyzeFile(filename);
                printStats(stats, filename, options);
            } catch (const exception& e) {
                cerr << "Ошибка при обработке файла '" << filename << "': " << e.what() << endl;
            }
        }

        // Пауза перед выходом
        cout << "Нажмите Enter для выхода...";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

    } catch (const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }
    return 0;
}
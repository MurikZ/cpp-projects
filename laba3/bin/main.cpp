#include "../lib/Sandpile.h"
#include <iostream>
#include <filesystem>
#include <optional>
#include <string_view>

int main(int argc, char* argv[]) {
    try {
        // Аргументы командной строки
        std::optional<int> numRows, numCols, iterationCap, snapshotStep;
        std::string sourcePath, resultPath;

        for (int i = 1; i < argc; ++i) {
            std::string_view argument = argv[i];
            if (argument == "--length") {
                if (++i < argc) {
                    numRows = std::stoi(argv[i]);
                }
            } else if (argument == "--width") {
                if (++i < argc) {
                    numCols = std::stoi(argv[i]);
                }
            } else if (argument == "--input") {
                if (++i < argc) {
                    sourcePath = argv[i];
                }
            } else if (argument == "--output") {
                if (++i < argc) {
                    resultPath = argv[i];
                }
            } else if (argument == "--max-iter") {
                if (++i < argc) {
                    iterationCap = std::stoi(argv[i]);
                }
            } else if (argument == "--freq") {
                if (++i < argc) {
                    snapshotStep = std::stoi(argv[i]);
                }
            }
        }

        // Проверка наличия всех необходимых параметров
        if (!numRows || !numCols || !iterationCap || !snapshotStep || sourcePath.empty() || resultPath.empty()) {
            std::cerr << "Недостаточно параметров. Правила использования: "
                      << argv[0] << " --length <int> --width <int> "
                      << "--input <file> --output <dir> --max-iter <int> --freq <int>\n";
            return 1;
        }

        // Инициализация симулятора
        GrainSimulator simulator(*numCols, *numRows);

        // Импорт данных
        simulator.importData(sourcePath);

        // Запуск симуляции
        simulator.execute(*iterationCap, *snapshotStep, resultPath);

        // Уведомление об успешном завершении
        std::cout << "Результат сохранён в " << resultPath << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

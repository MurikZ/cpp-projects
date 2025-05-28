#include "Sandpile.h"
#include "BmpWriter.h"
#include <fstream>
#include <sstream>
#include <filesystem>

// Конструктор - инициализация сетки заданного размера
GrainSimulator::GrainSimulator(int columns, int rows)
    : columns(columns), rows(rows) {
    // Создаем сетку rows x columns, заполненную нулями
    grid.assign(rows, std::vector<uint64_t>(columns, 0));//assign создает новый вектор с новыми значениями
}

// Загрузка начального состояния из файла
void GrainSimulator::importData(const std::string& path) {
    // Открываем файл для чтения
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + path);
    }
    
    std::string line;
    // Читаем файл построчно
    while (std::getline(in, line)) {
        std::istringstream iss(line);//читает данные из строки
        int x, y;          // Координаты ячейки
        uint64_t grains;   // Количество песчинок
        
        // Парсим строку формата "x y количество"
        if (!(iss >> x >> y >> grains)) continue;//проверяем на правильность значения x,y,grains
        
        // Проверяем, что координаты в пределах сетки
        if (y >= rows || x >= columns) continue;//если координаты за пределами сетки, пропускаем пропускаем такие строки
        
        // Добавляем песчинки в указанную ячейку
        grid[y][x] += grains;
    }
}

// Проверка, достигла ли система равновесия
bool GrainSimulator::checkEquilibrium() const {
    for (const auto& row : grid) {//проходимся по каждой строке
        for (uint64_t cell : row) {//проходимся по каждому элементу строки
        
            if (cell > 3) return false;//больше 3, система нестабильна
        }
    }
    return true;  // Все ячейки стабильны
}

// Перераспределение песчинок согласно правилам модели
void GrainSimulator::redistribute() {
    // Создаем временную копию сетки для нового состояния
    auto next = grid;
    
    // Проходим по всем ячейкам сетки
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            // Если в ячейке 4 или более песчинок
            if (grid[y][x] >= 4) {
                // Убираем 4 песчинки из текущей ячейки
                next[y][x] -= 4;
                
                // Распределяем по 1 песчинке в каждую соседнюю ячейку
                // (если сосед существует, т.е. не выходим за границы сетки)
                
                // Верхний сосед
                if (y > 0) next[y - 1][x]++;
                
                // Нижний сосед
                if (y + 1 < rows) next[y + 1][x]++;
                
                // Левый сосед
                if (x > 0) next[y][x - 1]++;
                
                // Правый сосед
                if (x + 1 < columns) next[y][x + 1]++;
            }
        }
    }
    
    // Заменяем текущую сетку новой (используем move для оптимизации) move не копирует, а просто передает 
    grid = std::move(next);
}

// Основной цикл симуляции
void GrainSimulator::execute(uint64_t maxIterations,
                           uint64_t freq,
                           const std::string& sourcePath) {
    // Выполняем заданное количество итераций
    for (uint64_t i = 0; i < maxIterations; ++i) {
        // Если задана частота сохранения и пришло время сохранять
        if (freq > 0 && i % freq == 0) {
            // Сохраняем текущее состояние с номером итерации в имени файла для наглядности действия
            exportBitmap(sourcePath + "_" + std::to_string(i));
        }
        
        // Проверяем, достигнуто ли равновесие
        if (checkEquilibrium()) break;
        
        // Выполняем шаг перераспределения
        redistribute();
    }
    
    // Сохраняем финальное состояние
    exportBitmap(sourcePath);
}

// Экспорт текущего состояния в BMP файл
void GrainSimulator::exportBitmap(const std::string& sourcePath) const {
    // Получаем базовое имя файла (без расширения)
    std::string base = std::filesystem::path(sourcePath).stem().string();
    
    // Вызываем функцию экспорта из BmpWriter
    BitmapExporter::exportBitmap(base + ".bmp", grid);
}

// Получение текущего состояния сетки
const std::vector<std::vector<uint64_t>>& GrainSimulator::getGrid() const {
    return grid;
}
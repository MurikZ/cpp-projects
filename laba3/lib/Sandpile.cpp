#include "Sandpile.h"
#include "BmpWriter.h"
#include <fstream>
#include <sstream>
#include <filesystem>

GrainSimulator::GrainSimulator(int columns, int rows)
    : columns(columns), rows(rows) {
    grid.assign(rows, std::vector<uint64_t>(columns, 0));
}

void GrainSimulator::importData(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + path);
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        int x, y;
        uint64_t grains;
        if (!(iss >> x >> y >> grains)) continue;
        if (y >= rows || x >= columns) continue;
        grid[y][x] += grains;
    }
}

bool GrainSimulator::checkEquilibrium() const {
    for (const auto& row : grid)
        for (uint64_t cell : row)
            if (cell > 3) return false;
    return true;
}

void GrainSimulator::redistribute() {
    auto next = grid;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            if (grid[y][x] >= 4) {
                next[y][x] -= 4;
                if (y > 0)         next[y - 1][x]++;
                if (y + 1 < rows)  next[y + 1][x]++;
                if (x > 0)         next[y][x - 1]++;
                if (x + 1 < columns) next[y][x + 1]++;
            }
        }
    }
    grid = std::move(next);
}

void GrainSimulator::execute(uint64_t maxIterations,
                             uint64_t freq,
                             const std::string& sourcePath) {
    for (uint64_t i = 0; i < maxIterations; ++i) {
        if (freq > 0 && i % freq == 0) {
            exportBitmap(sourcePath + "_" + std::to_string(i));
        }
        if (checkEquilibrium()) break;
        redistribute();
    }
    exportBitmap(sourcePath);
}

void GrainSimulator::exportBitmap(const std::string& sourcePath) const {
    std::string base = std::filesystem::path(sourcePath).stem().string();
    BitmapExporter::exportBitmap(base + ".bmp", grid);
}

const std::vector<std::vector<uint64_t>>& GrainSimulator::getGrid() const {
    return grid;
}
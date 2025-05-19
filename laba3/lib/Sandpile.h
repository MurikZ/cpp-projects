#pragma once
#include <vector>
#include <string>
#include <cstdint>

class GrainSimulator {
public:
    GrainSimulator(int columns, int rows);
    void importData(const std::string& path);
    void execute(uint64_t maxIterations = 100000,
                 uint64_t freq = 0,
                 const std::string& sourcePath = "");
    const std::vector<std::vector<uint64_t>>& getGrid() const;

private:
    void redistribute();
    bool checkEquilibrium() const;
    void exportBitmap(const std::string& sourcePath) const;

    int columns;
    int rows;
    std::vector<std::vector<uint64_t>> grid;
};
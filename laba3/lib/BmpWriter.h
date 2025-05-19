#pragma once
#include <vector>
#include <string>
#include <cstdint>

class BitmapExporter {
public:
    static void exportBitmap(const std::string& outputPath, const std::vector<std::vector<uint64_t>>& gridData);
};
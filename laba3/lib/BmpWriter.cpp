#include "BmpWriter.h"
#include <fstream>
#include <array>

void BitmapExporter::exportBitmap(const std::string& outputPath,
                                  const std::vector<std::vector<uint64_t>>& gridData) {
    int height = gridData.size();
    int width  = gridData[0].size();
    int rowStride  = (3 * width + 3) & ~3;
    int dataLength = rowStride * height;
    int totalSize  = 54 + dataLength;

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Не удалось открыть BMP-файл для записи.");
    }

    std::array<uint8_t, 54> bmpHeader = {
        'B','M',
        static_cast<uint8_t>(totalSize), static_cast<uint8_t>(totalSize >> 8),
        static_cast<uint8_t>(totalSize >> 16), static_cast<uint8_t>(totalSize >> 24),
        0,0, 0,0,
        54,0,0,0, 40,0,0,0,
        static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8),
        static_cast<uint8_t>(width >> 16), static_cast<uint8_t>(width >> 24),
        static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8),
        static_cast<uint8_t>(height >> 16), static_cast<uint8_t>(height >> 24),
        1,0, 24,0,
        0,0,0,0,
        static_cast<uint8_t>(dataLength), static_cast<uint8_t>(dataLength >> 8),
        static_cast<uint8_t>(dataLength >> 16), static_cast<uint8_t>(dataLength >> 24),
        0,0,0,0, 0,0,0,0,
        0,0,0,0, 0,0,0,0
    };

    outFile.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            uint8_t r = 255, g = 255, b = 255;
            uint64_t val = gridData[y][x];
            if (val == 1)      { r = 0;   g = 255; b = 0;   }
            else if (val == 2) { r = 255; g = 0;   b = 255; }
            else if (val == 3) { r = 255; g = 255; b = 0;   }
            else if (val > 3)  { r = 0;   g = 0;   b = 0;   }
            outFile.put(b).put(g).put(r);
        }
        for (int pad = 0; pad < rowStride - width * 3; ++pad)
            outFile.put(0);
    }
}
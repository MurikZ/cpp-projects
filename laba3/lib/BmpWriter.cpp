#include "BmpWriter.h"
#include <fstream>
#include <array>

// Метод для экспорта данных сетки в BMP файл
void BitmapExporter::exportBitmap(const std::string& outputPath,
                                 const std::vector<std::vector<uint64_t>>& gridData) {
    // 1. Подготовка параметров изображения
    int height = gridData.size();    // Высота = количество строк
    int width = gridData[0].size();  // Ширина = количество столбцов
    
    // Выравнивание строк по 4 байта (требование формата BMP)
    int rowStride = (3 * width + 3) & ~3;
    int dataLength = rowStride * height;  // Общий размер данных пикселей
    int totalSize = 54 + dataLength;     // Полный размер файла (54 байта заголовок + данные)

    // 2. Создание и проверка файла
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Не удалось открыть BMP-файл для записи.");
    }

    // 3. Формирование BMP заголовка (54 байта)
    std::array<uint8_t, 54> bmpHeader = {
        // Сигнатура формата
        'B','M',  // Магические числа формата BMP
        
        // Размер файла (4 байта, little-endian)
        static_cast<uint8_t>(totalSize), static_cast<uint8_t>(totalSize >> 8),
        static_cast<uint8_t>(totalSize >> 16), static_cast<uint8_t>(totalSize >> 24),
        
        // Зарезервированные поля (4 байта)
        0,0, 0,0,
        
        // Смещение до данных пикселей (4 байта)
        54,0,0,0,
        
        // Размер DIB заголовка (4 байта)
        40,0,0,0,
        
        // Ширина изображения (4 байта)
        static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8),
        static_cast<uint8_t>(width >> 16), static_cast<uint8_t>(width >> 24),
        
        // Высота изображения (4 байта)
        static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8),
        static_cast<uint8_t>(height >> 16), static_cast<uint8_t>(height >> 24),
        
        // Плоскости (1) и бит на пиксель (24) (по 2 байта)
        1,0, 24,0,
        
        // Сжатие и другие параметры (16 байт)
        0,0,0,0,  // Без сжатия
        static_cast<uint8_t>(dataLength), static_cast<uint8_t>(dataLength >> 8),
        static_cast<uint8_t>(dataLength >> 16), static_cast<uint8_t>(dataLength >> 24),
        0,0,0,0,  // Горизонтальное разрешение
        0,0,0,0,  // Вертикальное разрешение
        0,0,0,0,  // Цвета в палитре
        0,0,0,0   // Важные цвета
    };

    // 4. Запись заголовка в файл
    outFile.write(reinterpret_cast<char*>(bmpHeader.data()), bmpHeader.size());

    // 5. Запись данных пикселей (снизу вверх - особенность BMP)
    for (int y = height - 1; y >= 0; --y) {
        // Обработка каждой строки изображения
        for (int x = 0; x < width; ++x) {
            // Цвет по умолчанию - белый (255,255,255)
            uint8_t r = 255, g = 255, b = 255;
            uint64_t val = gridData[y][x];
            
            // Цветовая схема для разных значений:
            if (val == 1) {       // 1 песчинка - зеленый
                r = 0; g = 255; b = 0;
            }
            else if (val == 2) {  // 2 песчинки - фиолетовый
                r = 255; g = 0; b = 255;
            }
            else if (val == 3) {  // 3 песчинки - желтый
                r = 255; g = 255; b = 0;
            }
            else if (val > 3) {    // >3 песчинок - черный
                r = 0; g = 0; b = 0;
            }
            
            // Запись цвета пикселя (в порядке BGR)
            outFile.put(b).put(g).put(r);
        }
        
        // Дополнение строки до выравнивания по 4 байта
        for (int pad = 0; pad < rowStride - width * 3; ++pad)
            outFile.put(0);
    }
}
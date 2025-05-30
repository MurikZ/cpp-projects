#include "id3v2_parser.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mp3_file>" << std::endl;
        return 1;
    }

    Id3v2Parser parser(argv[1]);
    if (!parser.Parse()) {
        std::cerr << "Failed to parse ID3v2 tags" << std::endl;
        return 1;
    }

    parser.PrintTags();
    return 0;
}
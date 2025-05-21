#include <iostream>
#include <fstream>
#include <sstream>
#include "lib/parser.h"

using namespace confx;

int main(int argc, char* argv[]) {
    // Determine path to config file
    std::string filePath = "../example/config.omfl";
    if (argc > 1) {
        filePath = argv[1];
    }

    // Read entire file
    std::ifstream in(filePath);
    if (!in) {
        std::cerr << "Unable to open config: " << filePath << std::endl;
        return 1;
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    std::string text = buf.str();

    // Load config
    Configuration config = Load(text);
    if (!config.Valid()) {
        std::cerr << "Config load failed." << std::endl;
        return 2;
    }

    try {
        int rev       = config.Find("rev").ToInteger();
        std::string title  = config.Find("title").ToString();
        bool on        = config
                            .Find("functions")
                            .AsNode()
                            .Find("on")
                            .ToBoolean();

        std::string learner = config
                            .Find("institute")
                            .AsNode()
                            .Find("division")
                            .AsNode()
                            .Find("learner")
                            .ToString();

        std::cout << "institute >> division >> learner: " << learner << std::endl;
        std::cout << "Rev: "   << rev   << std::endl;
        std::cout << "Title: " << title << std::endl;
        std::cout << "Functions.on: " << (on ? "true" : "false") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error querying config: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}

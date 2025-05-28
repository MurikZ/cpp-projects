#include <iostream>
#include <fstream>
#include <sstream>
#include "lib/parser.h"

using namespace confx;

int main(int argc, char* argv[]) {
    // определяем файл конфигурации
    std::string filePath = "../../example/config.omfl";
    if (argc > 1) {
        filePath = argv[1];
    }

    //читаем файл
    std::ifstream in(filePath);
    if (!in) {
        std::cerr << "ошибка при открытии конфигурации: " << filePath << std::endl;
        return 1;
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    std::string text = buf.str();

    // загружаем конфигурацию
    Configuration config = Load(text);
    if (!config.Valid()) {//проверка на валидность
        std::cerr << "ошибка загрузки конфигурации." << std::endl;
        return 2;
    }

    try {
        int rev = config.Find("rev").ToInteger();
        std::string title  = config.Find("title").ToString();
        bool on  = config
                            .Find("functions")
                            .AsNode()
                            .Find("on")
                            .ToBoolean();

        std::string author = config
                            .Find("institute")
                            .AsNode()
                            .Find("division")
                            .AsNode()
                            .Find("author")
                            .ToString();

        std::cout << "institute >> division >> author: " << author << std::endl;
        std::cout << "Rev: " << rev << std::endl;
        std::cout << "Title: " << title << std::endl;
        std::cout << "Functions.on: " << (on ? "true" : "false") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ошибка запроса конфигурации: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}

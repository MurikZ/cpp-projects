# Аргумент-парсер на C++

Эта программа реализует простую, но гибкую библиотеку для парсинга аргументов командной строки на языке C++. Она позволяет удобно работать с опциями, флагами и позиционными аргументами.

## Возможности

- Поддержка именованных опций с обязательным или необязательным значением.
- Поддержка позиционных аргументов в нужном порядке.
- Обработка аргументов `argc` / `argv` в одном вызове.
- Доступ к значению любого аргумента по имени или индексу.
- Гибкое добавление новых типов аргументов при необходимости.
- Разделение логики по отдельным классам — легко расширять и тестировать.


## Пример использования

```cpp
#include "ArgParser.h"

int main(int argc, char** argv) {
    ArgParser parser;
    parser.addOption(new Option("input", "default.txt", true));
    parser.addFlag(new Flag("verbose"));
    parser.addPositional(new Positional(0));

    parser.parse(argc, argv);

    std::string inputFile = parser.getValue("input");
    bool verbose = parser.getFlag("verbose");
    std::string command = parser.getPositional(0);

    // ... дальнейшая логика
}
```



## Сборка и запуск

Для компиляции используйте любой компилятор, поддерживающий C++17 или выше:

```bash
g++ -std=c++17 bin/main.cpp lib/*.cpp -o argparser
./argparser --input data.txt run
```

## Лицензия

MIT
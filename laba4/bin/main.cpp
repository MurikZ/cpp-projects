#include <iostream>
#include <numeric>
#include <lib/ArgParser.h>

struct Options {
    bool sumMode = false;
    bool multMode = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    CommandLine::CmdParser parser("CmdTool");
    parser.AddIntParam("N").Multiple(1).PositionalParam().SaveValues(values);
    parser.AddFlagParam("sum", "add args").SaveValue(opt.sumMode);
    parser.AddFlagParam("mult", "multiply args").SaveValue(opt.multMode);
    parser.AddHelpParam('h', "help", "Show usage");

    if (!parser.Parse(argc, argv)) {
        std::cout << "Wrong argument\n";
        std::cout << parser.Usage() << std::endl;
        return 1;
    }

    if (parser.ShowHelp()) {
        std::cout << parser.Usage() << std::endl;
        return 0;
    }

    if (opt.sumMode) {
        std::cout << "Result: "
                  << std::accumulate(values.begin(), values.end(), 0)
                  << std::endl;
    } else if (opt.multMode) {
        std::cout << "Result: "
                  << std::accumulate(values.begin(), values.end(),
                                     1, std::multiplies<int>())
                  << std::endl;
    } else {
        std::cout << "No option selected\n";
        std::cout << parser.Usage();
        return 1;
    }

    return 0;
}

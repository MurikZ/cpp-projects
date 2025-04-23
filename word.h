#pragma once
#include <string>
#include <set>
using namespace std;

struct Stats {
    size_t lines = 0;
    size_t words = 0;
    size_t chars = 0;
    size_t bytes = 0;
};

Stats analyzeFile(const string& filename);
void printStats(const Stats& stats, const string& filename, const set<char>& options);

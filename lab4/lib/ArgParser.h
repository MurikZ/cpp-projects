#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CommandLine {

class CmdParser {
public:
    explicit CmdParser(const std::string& name);

    // Parsing entrypoints
    bool Parse(int argc, char** argv);
    bool Parse(const std::vector<std::string>& args);

    // Help/usage
    bool ShowHelp() const;
    std::string Usage() const;

    // Fetch parsed values
    std::string FetchString(const std::string& key) const;
    int         FetchInt(const std::string& key) const;
    int         FetchInt(const std::string& key, size_t idx) const;
    bool        FetchFlag(const std::string& key) const;

    // Builder
    class Builder;

    // Registration methods
    Builder AddStringParam(const std::string& key, const std::string& desc = "");
    Builder AddStringParam(char shortKey, const std::string& key, const std::string& desc = "");

    Builder AddIntParam(const std::string& key, const std::string& desc = "");
    Builder AddIntParam(char shortKey, const std::string& key, const std::string& desc = "");

    Builder AddFlagParam(const std::string& key, const std::string& desc = "");
    Builder AddFlagParam(char shortKey, const std::string& key, const std::string& desc = "");

    Builder AddHelpParam(char shortKey, const std::string& key, const std::string& desc);

private:
    std::string name_;
    bool helpFlag_ = false;

    enum class Type { String, Int, Flag, Help };
    struct Entry {
        Type type;
        std::string keyName;
        char keyShort;
        std::string desc;
        bool isPositional = false;
        bool allowMultiple = false;
        size_t minCount = 0;
        bool isSet = false;
        bool hasDefault = false;
        std::string defString;
        int defInt = 0;
        bool defBool = false;
        std::string valString;
        std::vector<std::string> valStrings;
        int valInt = 0;
        std::vector<int> valInts;
        bool valBool = false;
        std::string* ptrString = nullptr;
        std::vector<std::string>* ptrStrings = nullptr;
        int* ptrInt = nullptr;
        std::vector<int>* ptrInts = nullptr;
        bool* ptrBool = nullptr;
    };

    std::vector<Entry> entries_;
    std::unordered_map<std::string, size_t> longMap_;
    std::unordered_map<char, size_t> shortMap_;

    Entry& RegisterOption(Type type, char shortKey,
                          const std::string& keyName,
                          const std::string& desc);

public:
    class Builder {
    public:
        Builder(CmdParser& parent, size_t idx);

        Builder& WithDefault(const std::string& defVal);
        Builder& WithDefault(int defVal);
        Builder& WithDefault(bool defVal);

        Builder& SaveValue(std::string& out);
        Builder& SaveValues(std::vector<std::string>& out);
        Builder& SaveValue(int& out);
        Builder& SaveValues(std::vector<int>& out);
        Builder& SaveValue(bool& out);

        Builder& Multiple(size_t minCount = 0);
        Builder& PositionalParam();

    private:
        CmdParser& parser_;
        size_t idx_;
    };
};

} // namespace CommandLine

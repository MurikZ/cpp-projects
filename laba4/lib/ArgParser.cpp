#include "ArgParser.h"
#include <sstream>

namespace CommandLine {

CmdParser::CmdParser(const std::string& name)
    : name_(name) {}

CmdParser::Entry& CmdParser::RegisterOption(Type t, char sk,
                                            const std::string& kn,
                                            const std::string& d) {
    entries_.push_back({});
    Entry& e = entries_.back();
    e.type = t;
    e.keyName = kn;
    e.keyShort = sk;
    e.desc = d;
    if (t == Type::Flag || t == Type::Help) {
        e.valBool = false;
    }
    size_t idx = entries_.size() - 1;
    if (!kn.empty()) longMap_[kn] = idx;
    if (sk)       shortMap_[sk] = idx;
    return e;
}

// Builder implementations
CmdParser::Builder::Builder(CmdParser& p, size_t i)
    : parser_(p), idx_(i) {}

CmdParser::Builder& CmdParser::Builder::WithDefault(const std::string& dv) {
    auto& e = parser_.entries_[idx_];
    e.hasDefault = true;
    e.defString = dv;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::WithDefault(int dv) {
    auto& e = parser_.entries_[idx_];
    e.hasDefault = true;
    e.defInt = dv;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::WithDefault(bool dv) {
    auto& e = parser_.entries_[idx_];
    e.hasDefault = true;
    e.defBool = dv;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValue(std::string& out) {
    parser_.entries_[idx_].ptrString = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValues(std::vector<std::string>& out) {
    parser_.entries_[idx_].ptrStrings = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValue(int& out) {
    parser_.entries_[idx_].ptrInt = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValues(std::vector<int>& out) {
    parser_.entries_[idx_].ptrInts = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValue(bool& out) {
    parser_.entries_[idx_].ptrBool = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::Multiple(size_t mc) {
    auto& e = parser_.entries_[idx_];
    e.allowMultiple = true;
    e.minCount = mc;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::PositionalParam() {
    parser_.entries_[idx_].isPositional = true;
    return *this;
}

// Registration shortcuts
CmdParser::Builder CmdParser::AddStringParam(const std::string& k,
                                             const std::string& d) {
    RegisterOption(Type::String, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddStringParam(char sk,
                                             const std::string& k,
                                             const std::string& d) {
    RegisterOption(Type::String, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddIntParam(const std::string& k,
                                          const std::string& d) {
    RegisterOption(Type::Int, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddIntParam(char sk,
                                          const std::string& k,
                                          const std::string& d) {
    RegisterOption(Type::Int, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddFlagParam(const std::string& k,
                                           const std::string& d) {
    RegisterOption(Type::Flag, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddFlagParam(char sk,
                                           const std::string& k,
                                           const std::string& d) {
    RegisterOption(Type::Flag, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddHelpParam(char sk,
                                           const std::string& k,
                                           const std::string& d) {
    RegisterOption(Type::Help, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}

// Parsing core
bool CmdParser::Parse(const std::vector<std::string>& args) {
    for (auto& e : entries_) {
        e.isSet = false;
        e.valStrings.clear();
        e.valInts.clear();
    }
    helpFlag_ = false;

    Entry* posEntry = nullptr;
    for (auto& e : entries_)
        if (e.isPositional) { posEntry = &e; break; }

    for (size_t i = 1; i < args.size(); ++i) {
        const auto& a = args[i];
        if (a.rfind("--", 0) == 0) {
            if (a == "--help") {
                helpFlag_ = true;
                continue;
            }
            auto eq = a.find('=');
            std::string key = a.substr(2, eq - 2);
            if (!longMap_.count(key)) return false;
            Entry* e = &entries_[longMap_[key]];
            if (e->type == Type::Flag) {
                e->valBool = true;
                e->isSet = true;
            } else {
                if (eq == std::string::npos) return false;
                std::string v = a.substr(eq + 1);
                if (e->type == Type::String) {
                    if (e->allowMultiple) e->valStrings.push_back(v);
                    else                  e->valString = v;
                } else if (e->type == Type::Int) {
                    try {
                        int iv = std::stoi(v);
                        if (e->allowMultiple) e->valInts.push_back(iv);
                        else                  e->valInt = iv;
                    } catch (...) { return false; }
                }
                e->isSet = true;
            }
        } else if (a.rfind("-", 0) == 0) {
            if (a == "-h") {
                helpFlag_ = true;
                continue;
            }
            if (a.size() > 2 && a[2] == '=') {
                char sk = a[1];
                if (!shortMap_.count(sk)) return false;
                Entry* e = &entries_[shortMap_[sk]];
                std::string v = a.substr(3);
                if (e->type == Type::String) {
                    if (e->allowMultiple) e->valStrings.push_back(v);
                    else                  e->valString = v;
                } else if (e->type == Type::Int) {
                    try {
                        int iv = std::stoi(v);
                        if (e->allowMultiple) e->valInts.push_back(iv);
                        else                  e->valInt = iv;
                    } catch (...) { return false; }
                } else return false;
                e->isSet = true;
            } else {
                for (size_t k = 1; k < a.size(); ++k) {
                    char sk = a[k];
                    if (!shortMap_.count(sk)) return false;
                    Entry* e = &entries_[shortMap_[sk]];
                    if (e->type == Type::Flag) {
                        e->valBool = true;
                        e->isSet = true;
                    } else return false;
                }
            }
        } else {
            if (!posEntry) return false;
            Entry* e = posEntry;
            if (e->type == Type::String) {
                if (e->allowMultiple) e->valStrings.push_back(a);
                else                  e->valString = a;
            } else if (e->type == Type::Int) {
                try {
                    int iv = std::stoi(a);
                    if (e->allowMultiple) e->valInts.push_back(iv);
                    else                  e->valInt = iv;
                } catch (...) { return false; }
            } else return false;
            e->isSet = true;
        }
    }

    if (helpFlag_) return true;

    for (auto& e : entries_) {
        if (e.type == Type::String) {
            if (!e.allowMultiple) {
                if (!e.isSet) {
                    if (e.hasDefault) e.valString = e.defString;
                    else              return false;
                }
            } else if (e.valStrings.size() < e.minCount) return false;
            if (e.ptrString)  *e.ptrString  = e.valString;
            if (e.ptrStrings) *e.ptrStrings = e.valStrings;
        } else if (e.type == Type::Int) {
            if (!e.allowMultiple) {
                if (!e.isSet) {
                    if (e.hasDefault) e.valInt = e.defInt;
                    else              return false;
                }
            } else if (e.valInts.size() < e.minCount) return false;
            if (e.ptrInt)  *e.ptrInt  = e.valInt;
            if (e.ptrInts) *e.ptrInts = e.valInts;
        } else if (e.type == Type::Flag) {
            if (!e.isSet && e.hasDefault) e.valBool = e.defBool;
            if (e.ptrBool) *e.ptrBool = e.valBool;
        }
    }

    return true;
}

bool CmdParser::Parse(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);
    return Parse(args);
}

bool CmdParser::ShowHelp() const {
    return helpFlag_;
}

std::string CmdParser::Usage() const {
    std::ostringstream out;
    out << name_ << "\n";
    return out.str();
}

std::string CmdParser::FetchString(const std::string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return {};
    const Entry& e = entries_[it->second];
    if (!e.allowMultiple && !e.isSet && e.hasDefault) return e.defString;
    if (e.allowMultiple) return e.valStrings.empty() ? "" : e.valStrings[0];
    return e.valString;
}

int CmdParser::FetchInt(const std::string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return 0;
    const Entry& e = entries_[it->second];
    if (!e.allowMultiple && !e.isSet && e.hasDefault) return e.defInt;
    if (e.allowMultiple) return e.valInts.empty() ? 0 : e.valInts[0];
    return e.valInt;
}

int CmdParser::FetchInt(const std::string& k, size_t idx) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return 0;
    const Entry& e = entries_[it->second];
    return (e.allowMultiple && idx < e.valInts.size()) ? e.valInts[idx] : 0;
}

bool CmdParser::FetchFlag(const std::string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return false;
    const Entry& e = entries_[it->second];
    return e.isSet ? e.valBool : (e.hasDefault ? e.defBool : false);
}

} // namespace CommandLine

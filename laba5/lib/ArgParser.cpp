#include "ArgParser.h"
#include <sstream>

namespace CommandLine {
using namespace std;
// Конструктор: сохраняет имя парсера (например, имя программы)
CmdParser::CmdParser(const  string& name)
    : name_(name) {}

// Регистрация новой опции
CmdParser::Entry& CmdParser::RegisterOption(Type t, char sk,
                                            const  string& kn,
                                            const  string& d) {
    entries_.push_back({});
    Entry& e = entries_.back();
    e.type = t;
    e.keyName = kn;     // длинное имя параметра (--key)
    e.keyShort = sk;    // короткое имя параметра (-k)
    e.desc = d;         // описание
    if (t == Type::Flag || t == Type::Help) {
        e.valBool = false;
    }

    size_t idx = entries_.size() - 1;
    if (!kn.empty()) longMap_[kn] = idx;  // индекс по длинному имени
    if (sk)          shortMap_[sk] = idx; // индекс по короткому имени
    return e;
}



CmdParser::Builder::Builder(CmdParser& p, size_t i)
    : parser_(p), idx_(i) {}

// Задание значения по умолчанию (строка, число, флаг)
CmdParser::Builder& CmdParser::Builder::WithDefault(const  string& dv) {
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

// Сохраняем указатели на переменные, в которые будет записан результат
CmdParser::Builder& CmdParser::Builder::SaveValue( string& out) {
    parser_.entries_[idx_].ptrString = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValues( vector< string>& out) {
    parser_.entries_[idx_].ptrStrings = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValue(int& out) {
    parser_.entries_[idx_].ptrInt = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValues( vector<int>& out) {
    parser_.entries_[idx_].ptrInts = &out;
    return *this;
}

CmdParser::Builder& CmdParser::Builder::SaveValue(bool& out) {
    parser_.entries_[idx_].ptrBool = &out;
    return *this;
}

// Разрешить множественные значения
CmdParser::Builder& CmdParser::Builder::Multiple(size_t mc) {
    auto& e = parser_.entries_[idx_];
    e.allowMultiple = true;
    e.minCount = mc;  // минимальное количество значений
    return *this;
}

// Указать, что это позиционный параметр (без ключа)
CmdParser::Builder& CmdParser::Builder::PositionalParam() {
    parser_.entries_[idx_].isPositional = true;
    return *this;
}

// ===================== Быстрые методы регистрации опций ===================== //

CmdParser::Builder CmdParser::AddStringParam(const  string& k, const  string& d) {
    RegisterOption(Type::String, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddStringParam(char sk, const  string& k, const  string& d) {
    RegisterOption(Type::String, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}

CmdParser::Builder CmdParser::AddIntParam(const  string& k, const  string& d) {
    RegisterOption(Type::Int, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddIntParam(char sk, const  string& k, const  string& d) {
    RegisterOption(Type::Int, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}

CmdParser::Builder CmdParser::AddFlagParam(const  string& k, const  string& d) {
    RegisterOption(Type::Flag, 0, k, d);
    return Builder(*this, entries_.size() - 1);
}
CmdParser::Builder CmdParser::AddFlagParam(char sk, const  string& k, const  string& d) {
    RegisterOption(Type::Flag, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}

CmdParser::Builder CmdParser::AddHelpParam(char sk, const  string& k, const  string& d) {
    RegisterOption(Type::Help, sk, k, d);
    return Builder(*this, entries_.size() - 1);
}

// ===================== Парсинг аргументов ===================== //

bool CmdParser::Parse(const  vector< string>& args) {
    // Обнуляем значения
    for (auto& e : entries_) {
        e.isSet = false;
        e.valStrings.clear();
        e.valInts.clear();
    }
    helpFlag_ = false;

    // Ищем позиционный параметр
    Entry* posEntry = nullptr;
    for (auto& e : entries_)
        if (e.isPositional) { posEntry = &e; break; }

    // Основной цикл обработки аргументов
    for (size_t i = 1; i < args.size(); ++i) {
        const auto& a = args[i];

        // Длинный ключ (--key=value)
        if (a.rfind("--", 0) == 0) {
            if (a == "--help") { helpFlag_ = true; continue; }
            auto eq = a.find('=');
             string key = a.substr(2, eq - 2);
            if (!longMap_.count(key)) return false;
            Entry* e = &entries_[longMap_[key]];

            if (e->type == Type::Flag) {
                e->valBool = true;
                e->isSet = true;
            } else {
                if (eq ==  string::npos) return false;
                 string v = a.substr(eq + 1);
                if (e->type == Type::String) {
                    if (e->allowMultiple) e->valStrings.push_back(v);
                    else e->valString = v;
                } else if (e->type == Type::Int) {
                    try {
                        int iv =  stoi(v);
                        if (e->allowMultiple) e->valInts.push_back(iv);
                        else e->valInt = iv;
                    } catch (...) { return false; }
                }
                e->isSet = true;
            }

        // Короткий ключ (-k=value или -f)
        } else if (a.rfind("-", 0) == 0) {
            if (a == "-h") { helpFlag_ = true; continue; }

            // Формат -k=value
            if (a.size() > 2 && a[2] == '=') {
                char sk = a[1];
                if (!shortMap_.count(sk)) return false;
                Entry* e = &entries_[shortMap_[sk]];
                 string v = a.substr(3);
                if (e->type == Type::String) {
                    if (e->allowMultiple) e->valStrings.push_back(v);
                    else e->valString = v;
                } else if (e->type == Type::Int) {
                    try {
                        int iv =  stoi(v);
                        if (e->allowMultiple) e->valInts.push_back(iv);
                        else e->valInt = iv;
                    } catch (...) { return false; }
                } else return false;
                e->isSet = true;

            // Формат -abc (несколько флагов подряд)
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

        // Позиционный аргумент (без ключа)
        } else {
            if (!posEntry) return false;
            Entry* e = posEntry;
            if (e->type == Type::String) {
                if (e->allowMultiple) e->valStrings.push_back(a);
                else e->valString = a;
            } else if (e->type == Type::Int) {
                try {
                    int iv =  stoi(a);
                    if (e->allowMultiple) e->valInts.push_back(iv);
                    else e->valInt = iv;
                } catch (...) { return false; }
            } else return false;
            e->isSet = true;
        }
    }

    // Если был указан --help, завершаем
    if (helpFlag_) return true;

    // Проверка обязательных значений и установка значений по умолчанию
    for (auto& e : entries_) {
        if (e.type == Type::String) {
            if (!e.allowMultiple && !e.isSet) {
                if (e.hasDefault) e.valString = e.defString;
                else return false;
            } else if (e.valStrings.size() < e.minCount) return false;
            if (e.ptrString) *e.ptrString = e.valString;
            if (e.ptrStrings) *e.ptrStrings = e.valStrings;
        } else if (e.type == Type::Int) {
            if (!e.allowMultiple && !e.isSet) {
                if (e.hasDefault) e.valInt = e.defInt;
                else return false;
            } else if (e.valInts.size() < e.minCount) return false;
            if (e.ptrInt) *e.ptrInt = e.valInt;
            if (e.ptrInts) *e.ptrInts = e.valInts;
        } else if (e.type == Type::Flag) {
            if (!e.isSet && e.hasDefault) e.valBool = e.defBool;
            if (e.ptrBool) *e.ptrBool = e.valBool;
        }
    }

    return true;
}

// Обёртка над Parse
bool CmdParser::Parse(int argc, char** argv) {
     vector< string> args(argv, argv + argc);
    return Parse(args);
}

// Возврат состояния флага --help
bool CmdParser::ShowHelp() const {
    return helpFlag_;
}

// Строка с названием программы (можно дополнить usage позже)
 string CmdParser::Usage() const {
     ostringstream out;
    out << name_ << "\n";
    return out.str();
}

// Получение значения параметров
 string CmdParser::FetchString(const  string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return {};
    const Entry& e = entries_[it->second];
    if (!e.allowMultiple && !e.isSet && e.hasDefault) return e.defString;
    if (e.allowMultiple) return e.valStrings.empty() ? "" : e.valStrings[0];
    return e.valString;
}

int CmdParser::FetchInt(const  string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return 0;
    const Entry& e = entries_[it->second];
    if (!e.allowMultiple && !e.isSet && e.hasDefault) return e.defInt;
    if (e.allowMultiple) return e.valInts.empty() ? 0 : e.valInts[0];
    return e.valInt;
}

int CmdParser::FetchInt(const  string& k, size_t idx) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return 0;
    const Entry& e = entries_[it->second];
    return (e.allowMultiple && idx < e.valInts.size()) ? e.valInts[idx] : 0;
}

bool CmdParser::FetchFlag(const  string& k) const {
    auto it = longMap_.find(k);
    if (it == longMap_.end()) return false;
    const Entry& e = entries_[it->second];
    return e.isSet ? e.valBool : (e.hasDefault ? e.defBool : false);
}

} 
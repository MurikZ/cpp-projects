#include "parser.h"
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace confx {

// ---------- Node implementation ----------

const Element& Node::Find(const std::string& key) const {
    auto it = items_.find(key);
    if (it == items_.end()) throw std::runtime_error("Key not found: " + key);
    return it->second;
}

bool Node::Exists(const std::string& key) const {
    return items_.find(key) != items_.end();
}

// ---------- Element implementation ----------

#define CHECK_KIND(k) if (kind_ != Kind::k) throw std::runtime_error("Bad kind");

bool Element::IsInteger() const { return kind_ == Kind::INTEGER; }
bool Element::IsReal()    const { return kind_ == Kind::REAL; }
bool Element::IsBoolean() const { return kind_ == Kind::BOOLEAN; }
bool Element::IsText()    const { return kind_ == Kind::TEXT; }
bool Element::IsList()    const { return kind_ == Kind::LIST; }
bool Element::IsNode()    const { return kind_ == Kind::NODE; }

int Element::ToInteger()      const { CHECK_KIND(INTEGER); return valueInt_; }
float Element::ToReal()       const { CHECK_KIND(REAL);    return valueReal_; }
bool Element::ToBoolean()     const { CHECK_KIND(BOOLEAN); return valueBool_; }
const std::string& Element::ToString()  const { CHECK_KIND(TEXT);    return valueStr_; }
const std::vector<Element>& Element::ToList() const { CHECK_KIND(LIST);    return valueList_; }
const Node& Element::AsNode()           const { CHECK_KIND(NODE);    return *nodePtr_; }

Element Element::MakeInteger(int v)          { Element e; e.kind_ = Kind::INTEGER; e.valueInt_ = v; return e; }
Element Element::MakeReal(float v)           { Element e; e.kind_ = Kind::REAL;    e.valueReal_ = v; return e; }
Element Element::MakeBoolean(bool v)         { Element e; e.kind_ = Kind::BOOLEAN; e.valueBool_ = v; return e; }
Element Element::MakeText(const std::string& v) { Element e; e.kind_ = Kind::TEXT;   e.valueStr_ = v; return e; }
Element Element::MakeList(const std::vector<Element>& v) { Element e; e.kind_ = Kind::LIST; e.valueList_ = v; return e; }
Element Element::MakeNode(const Node& n)     { Element e; e.kind_ = Kind::NODE; e.nodePtr_ = new Node(n); return e; }

// ---------- Configuration implementation ----------

const Element& Configuration::Find(const std::string& key) const {
    return root_.Find(key);
}
bool Configuration::Exists(const std::string& key) const {
    return root_.Exists(key);
}

// ---------- Parser core ----------

static inline void strip(std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of (" \t\r\n");
    s = (a == std::string::npos ? "" : s.substr(a, b - a + 1));
}

Element interpret(const std::string& s);

Configuration Load(const std::string& text) {
    Configuration cfg;
    Node* current = &cfg.root_;
    std::istringstream in(text);
    std::string line;

    try {
        while (std::getline(in, line)) {
            bool inQuotes = false;
            std::string   clean;
            for (char c : line) {
                if (c == '"') inQuotes = !inQuotes;
                if (!inQuotes && c == '#') break;
                clean += c;
            }
            strip(clean);
            if (clean.empty()) continue;

            // Section header
            if (clean.front() == '[' && clean.back() == ']') {
                std::string path = clean.substr(1, clean.size()-2);
                strip(path);
                current = &cfg.root_;
                size_t pos = 0;
                while (pos < path.size()) {
                    size_t dot = path.find('.', pos);
                    std::string part = path.substr(pos, dot==std::string::npos?std::string::npos:dot-pos);
                    if (!current->Exists(part)) {
                        Node newNode;
                        current->items_[part] = Element::MakeNode(newNode);
                    }
                    current = const_cast<Node*>(&current->Find(part).AsNode());
                    if (dot==std::string::npos) break;
                    pos = dot+1;
                }
                continue;
            }

            // key = value
            size_t eq = clean.find('=');
            if (eq == std::string::npos) throw std::runtime_error("Invalid line");
            std::string key = clean.substr(0, eq);
            std::string val = clean.substr(eq+1);
            strip(key); strip(val);
            if (key.empty()||val.empty()) throw std::runtime_error("Empty key/value");
            Element e = interpret(val);
            if (current->Exists(key)) throw std::runtime_error("Duplicate key");
            current->items_[key] = std::move(e);
        }
        cfg.ok_ = true;
    } catch(...) {
        cfg.ok_ = false;
    }
    return cfg;
}

Element interpret(const std::string& s) {
    // String literal
    if (s.front()=='"' && s.back()=='"')
        return Element::MakeText(s.substr(1,s.size()-2));
    // Boolean
    if (s=="true"||s=="false")
        return Element::MakeBoolean(s=="true");
    // Array
    if (s.front()=='[' && s.back()==']') {
        std::vector<Element> list;
        std::string inner = s.substr(1,s.size()-2);
        int depth=0; size_t st=0;
        for (size_t i=0;i<=inner.size();++i) {
            if (i<inner.size()) {
                if (inner[i]=='[') depth++;
                if (inner[i]==']') depth--;
                if (inner[i]==',' && depth==0) {
                    std::string part = inner.substr(st,i-st);
                    strip(part);
                    list.push_back(interpret(part));
                    st = i+1;
                }
                continue;
            }
            std::string part = inner.substr(st);
            strip(part);
            if (!part.empty()) list.push_back(interpret(part));
        }
        return Element::MakeList(list);
    }
    // Number?
    if (s.find('.')==std::string::npos) {
        int v = std::stoi(s);
        return Element::MakeInteger(v);
    } else {
        float v = std::stof(s);
        return Element::MakeReal(v);
    }
}

} // namespace confx

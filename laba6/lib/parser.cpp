#include "parser.h"
#include <sstream>
#include <cctype>
#include <stdexcept>
using namespace std;
namespace confx {

// ---------- Node implementation ----------

const Element& Node::Find(const string& key) const {
    auto it = items.find(key);
    if (it == items.end()) throw runtime_error("Ключ не найден: " + key);//если элемент не найден
    return it->second;//возвращаем значение
}

bool Node::Exists(const string& key) const {
    return items.find(key) != items.end();//проверка существования ключа
}


#define CHECK_KIND(k) if (kind != Kind::k) throw runtime_error("Bad kind");

// Каждый метод проверяет соответствие типа элемента
bool Element::IsInteger() const { return kind == Kind::INTEGER; }
bool Element::IsReal()const { return kind == Kind::REAL; }
bool Element::IsBoolean() const { return kind == Kind::BOOLEAN; }
bool Element::IsText()const { return kind == Kind::TEXT; }
bool Element::IsList()const { return kind == Kind::LIST; }
bool Element::IsNode()const { return kind == Kind::NODE; }

// Каждый метод проверяет тип и возвращает соответствующее значение
int Element::ToInteger() const { CHECK_KIND(INTEGER); return valueInt; }
float Element::ToReal() const { CHECK_KIND(REAL);    return valueReal; }
bool Element::ToBoolean() const { CHECK_KIND(BOOLEAN); return valueBool; }
const string& Element::ToString()  const { CHECK_KIND(TEXT);    return valueStr; }
const vector<Element>& Element::ToList() const { CHECK_KIND(LIST);    return valueList; }
const Node& Element::AsNode()           const { CHECK_KIND(NODE);    return *nodePtr; }

//методы создания элементов
Element Element::MakeInteger(int v){ Element e; e.kind = Kind::INTEGER; e.valueInt = v; return e; }
Element Element::MakeReal(float v) { Element e; e.kind = Kind::REAL;    e.valueReal = v; return e; }
Element Element::MakeBoolean(bool v){ Element e; e.kind = Kind::BOOLEAN; e.valueBool = v; return e; }
Element Element::MakeText(const string& v) { Element e; e.kind = Kind::TEXT;   e.valueStr = v; return e; }
Element Element::MakeList(const vector<Element>& v) { Element e; e.kind = Kind::LIST; e.valueList = v; return e; }
Element Element::MakeNode(const Node& n) { Element e; e.kind = Kind::NODE; e.nodePtr = new Node(n); return e; }

// реализация класса Configuration

const Element& Configuration::Find(const string& key) const {
    return root.Find(key);
}
bool Configuration::Exists(const string& key) const {
    return root.Exists(key);
}

// парсер

static inline void strip(string& s) {//удаление пробелов
    size_t a = s.find_first_not_of(" \t\r\n");//первый непробельный символ
    size_t b = s.find_last_not_of (" \t\r\n");//последний непробельный символ
    s = (a == string::npos ? "" : s.substr(a, b - a + 1));
}

Element interpret(const string& s);

Configuration Load(const string& text) {
    Configuration cfg;
    Node* current = &cfg.root;
    istringstream in(text);
    string line;

    try {
        while (getline(in, line)) {
            bool inQuotes = false;
            string clean;
            for (char c : line) {
                if (c == '"') inQuotes = !inQuotes;
                if (!inQuotes && c == '#') break;
                clean += c;
            }
            strip(clean);
            if (clean.empty()) continue;

            // Section header
            if (clean.front() == '[' && clean.back() == ']') {
                string path = clean.substr(1, clean.size()-2);
                strip(path);
                current = &cfg.root;
                size_t pos = 0;
                while (pos < path.size()) {
                    size_t dot = path.find('.', pos);
                    string part = path.substr(pos, dot==string::npos?string::npos:dot-pos);
                    if (!current->Exists(part)) {
                        Node newNode;
                        current->items[part] = Element::MakeNode(newNode);
                    }
                    current = const_cast<Node*>(&current->Find(part).AsNode());
                    if (dot==string::npos) break;
                    pos = dot+1;
                }
                continue;
            }

            // key = value
            size_t eq = clean.find('=');
            if (eq == string::npos) throw runtime_error("неправильная строка");
            string key = clean.substr(0, eq);
            string val = clean.substr(eq+1);
            strip(key); strip(val);
            if (key.empty()||val.empty()) throw runtime_error("пустрой ключ/значение");
            Element e = interpret(val);
            if (current->Exists(key)) throw runtime_error("дупликат ключа");
            current->items[key] = move(e);
        }
        cfg.ok = true;
    } catch(...) {
        cfg.ok = false;
    }
    return cfg;
}
//преобразуем строковое значение в объект типа element
Element interpret(const string& s) {
    // String literal
    if (s.front()=='"' && s.back()=='"')
        return Element::MakeText(s.substr(1,s.size()-2));
    // Boolean
    if (s=="true"||s=="false")
        return Element::MakeBoolean(s=="true");
    // Array
    if (s.front()=='[' && s.back()==']') {
        vector<Element> list;
        string inner = s.substr(1,s.size()-2);
        int depth=0; size_t st=0;
        for (size_t i=0;i<=inner.size();++i) {
            if (i<inner.size()) {
                if (inner[i]=='[') depth++;
                if (inner[i]==']') depth--;
                if (inner[i]==',' && depth==0) {
                    string part = inner.substr(st,i-st);
                    strip(part);
                    list.push_back(interpret(part));
                    st = i+1;
                }
                continue;
            }
            string part = inner.substr(st);
            strip(part);
            if (!part.empty()) list.push_back(interpret(part));
        }
        return Element::MakeList(list);
    }
    // обработка чисел
    if (s.find('.')==string::npos) {
        int v = stoi(s);
        return Element::MakeInteger(v);
    } else {
        float v = stof(s);
        return Element::MakeReal(v);
    }
}

} // namespace confx

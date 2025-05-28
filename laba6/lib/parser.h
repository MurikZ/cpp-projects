#pragma once

#include <string>
#include <vector>
#include <map>

namespace confx {

// Forward
class Element;

// узел конфигурации, может содержать вложенные ключи
class Node {
public:
    const Element& Find(const std::string& key) const;
    bool Exists(const std::string& key) const;

    // storage for parser
    std::map<std::string, Element> items;
};

// значения OMFL
class Element {
public:
    // Type queries
    bool IsInteger()const;
    bool IsReal()const;
    bool IsBoolean()const;
    bool IsText()const;
    bool IsList()const;
    bool IsNode()const;

    // Value access
    int ToInteger() const;
    float ToReal() const;
    bool ToBoolean() const;
    const std::string& ToString()   const;
    const std::vector<Element>& ToList()  const;
    const Node& AsNode() const;

    // Internal constructors
    static Element MakeInteger(int v);
    static Element MakeReal(float v);
    static Element MakeBoolean(bool v);
    static Element MakeText(const std::string& v);
    static Element MakeList(const std::vector<Element>& v);
    static Element MakeNode(const Node& n);

private:
    enum class Kind { INTEGER, REAL, BOOLEAN, TEXT, LIST, NODE, NONE } kind{Kind::NONE};

    int valueInt{};
    float valueReal{};
    bool valueBool{};
    std::string valueStr;
    std::vector<Element> valueList;
    Node* nodePtr{};
};

// Whole–config with root node
class Configuration {
public:
    bool ok{false};
    bool Valid() const { return ok; }

    const Element& Find(const std::string& key) const;
    bool Exists(const std::string& key) const;

    // for parser
    Node root;
};

// Load OMFL text into Configuration
Configuration Load(const std::string& text);

}

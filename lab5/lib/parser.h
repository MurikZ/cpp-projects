#pragma once

#include <string>
#include <vector>
#include <map>

namespace confx {

// Forward
class Element;

// Node holds subsections & values
class Node {
public:
    const Element& Find(const std::string& key) const;
    bool            Exists(const std::string& key) const;

    // storage for parser
    std::map<std::string, Element> items_;
};

// Element wraps any OMFL value
class Element {
public:
    // Type queries
    bool IsInteger()  const;
    bool IsReal()     const;
    bool IsBoolean()  const;
    bool IsText()     const;
    bool IsList()     const;
    bool IsNode()     const;

    // Value access
    int                      ToInteger()  const;
    float                    ToReal()     const;
    bool                     ToBoolean()  const;
    const std::string&       ToString()   const;
    const std::vector<Element>& ToList()  const;
    const Node&              AsNode()     const;

    // Internal constructors
    static Element MakeInteger(int v);
    static Element MakeReal(float v);
    static Element MakeBoolean(bool v);
    static Element MakeText(const std::string& v);
    static Element MakeList(const std::vector<Element>& v);
    static Element MakeNode(const Node& n);

private:
    enum class Kind { INTEGER, REAL, BOOLEAN, TEXT, LIST, NODE, NONE } kind_{Kind::NONE};

    int valueInt_{};
    float valueReal_{};
    bool valueBool_{};
    std::string valueStr_;
    std::vector<Element> valueList_;
    Node* nodePtr_{};
};

// Whole–config with root node
class Configuration {
public:
    bool ok_{false};
    bool Valid() const { return ok_; }

    const Element& Find(const std::string& key) const;
    bool            Exists(const std::string& key) const;

    // for parser
    Node root_;
};

// Load OMFL text into Configuration
Configuration Load(const std::string& text);

} // namespace confx

#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
    class Node;
    using Array = std::vector<Node>;
    using Dict = std::map<std::string, Node>;
    using Var = std::variant<nullptr_t, Array, Dict, bool, int, double, std::string>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    using namespace std::literals;

    struct NodeValueVisitor {
        std::ostream& out;
        static int otstup;
        void operator()(std::nullptr_t);
        void operator()(Array array);
        void operator()(Dict dict);
        void operator()(bool b);
        void operator()(int val);
        void operator()(double val);
        void operator()(std::string str);
    };

    class Node : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        const Array& AsArray() const;
        const Dict& AsMap() const;
        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Value& GetValue() const;

        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsBool() const;
        bool IsArray() const;
        bool IsMap() const;

        bool operator==(const Node node) const;
        bool operator!=(const Node node) const;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document doc) const;
        bool operator!=(const Document doc) const;
    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);


}   // namespace json
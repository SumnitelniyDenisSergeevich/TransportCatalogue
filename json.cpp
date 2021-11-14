#include "json.h"
#include <unordered_map>

using namespace std;

namespace json {

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return get<Array>(*this);
        }
        throw logic_error("its not array"s);
    }
    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return get<Dict>(*this);
        }
        throw logic_error("its not dict"s);
    }
    bool Node::AsBool() const {
        if (IsBool()) {
            return get<bool>(*this);
        }
        throw logic_error("its not bool"s);
    }
    int Node::AsInt() const {
        if (IsInt()) {
            return get<int>(*this);
        }
        throw logic_error("its not int"s);
    }
    double Node::AsDouble() const {
        if (IsInt() || IsDouble()) {
            return holds_alternative<double>(*this) ? get<double>(*this) : get<int>(*this);
        }
        throw logic_error("its not double"s);
    }
    const std::string& Node::AsString() const {
        if (IsString()) {
            return get<string>(*this);
        }
        throw logic_error("its not string"s);
    }

    const Node::Value& Node::GetValue() const {
        return *this;
    }

    bool Node::IsNull() const {
        return holds_alternative<nullptr_t>(*this);
    }
    bool Node::IsInt() const {
        return holds_alternative<int>(*this);
    }
    bool Node::IsDouble() const {
        return holds_alternative<int>(*this) || holds_alternative<double>(*this);
    }
    bool Node::IsPureDouble() const {
        return holds_alternative<double>(*this);
    }
    bool Node::IsString() const {
        return holds_alternative<string>(*this);
    }
    bool Node::IsBool() const {
        return holds_alternative<bool>(*this);
    }
    bool Node::IsArray() const {
        return holds_alternative<Array>(*this);
    }
    bool Node::IsMap() const {
        return holds_alternative<Dict>(*this);
    }


    bool Node::operator==(const Node node) const {
        return this == &node;
    }
    bool Node::operator!=(const Node node) const {
        return !(*this == node);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document doc) const {
        return this->GetRoot() == doc.GetRoot();
    }
    bool Document::operator!=(const Document doc) const {
        return !(*this == doc);
    }
    namespace {
        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            for (char c; input >> c && c != ']';) {
                if (input.peek() == -1) {
                    throw ParsingError("LoadArray Error"s);
                }
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            return Node(move(result));
        }
        Node LoadString(istream& input) {
            static const std::unordered_map<char, char> escape_sequences{
        {'a','\a'},
        {'b','\b'},
        {'f','\f'},
        {'n','\n'},
        {'r','\r'},
        {'t','\t'},
        {'v','\v'},
        {'\'','\''},
        {'\"','\"'},
        {'\\','\\'}
            };
            string str;
            bool escaped = false;
            char c;
            for (; input.get(c) && !(!escaped && c == '\"');) {
                if (!escaped) {
                    if (c == '\\') {
                        escaped = true;
                    }
                    else {
                        str += c;
                    }
                }
                else {
                    if (const auto it = escape_sequences.find(c); it != escape_sequences.end()) {
                        str += it->second;
                    }
                    else {
                        throw ParsingError("Unknown escape sequence \\"s + std::to_string(c));
                    }
                    escaped = false;
                }
            }
            if ((escaped) || (c != '\"')) {
                throw ParsingError("Failed to read string. Could collect only ["s + str + "]"s);
            }
            return Node(move(str));
        }

        Node LoadDict(istream& input) {
            Dict result;

            for (char c; input >> c && c != '}';) {
                if (input.peek() == -1) {
                    throw ParsingError("LoadDict Error"s);
                }
                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            return Node(move(result));
        }

        //------------------------------------------------------------Number---------------------------------------------------

        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input) {

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }
            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }
        //------------------------------------------------------------END Number---------------------------------------------------

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == '[') {
                if (input.peek() == -1) {
                    throw ParsingError("LoadArray Error"s);
                }
                return LoadArray(input);
            }
            else if (c == '{') {
                if (input.peek() == -1) {
                    throw ParsingError("LoadDict Error"s);
                }
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                string null = "n"s;
                for (int i = 0; i < 3; ++i) {
                    if (input.peek() == -1) {
                        throw ParsingError("Failed, expected \"null\""s);
                    }
                    char c;
                    input.get(c);
                    null.push_back(c);
                }
                if (null == "null"s) {
                    return Node(nullptr);
                }
                else {
                    throw ParsingError("Failed to convert "s + null + " to null"s);
                }
            }
            else if (c == 't') {
                string true_str = "t"s;
                for (int i = 0; i < 3; ++i) {
                    if (input.peek() == -1) {
                        throw ParsingError("Failed, expected \"true\""s);
                    }
                    char c;
                    input.get(c);
                    true_str.push_back(c);
                }
                if (true_str == "true"s) {
                    return Node(true);
                }
                else {
                    throw ParsingError("Failed to convert "s + true_str + " to true"s);
                }
            }
            else if (c == 'f') {
                string false_str = "f"s;
                for (int i = 0; i < 4; ++i) {
                    if (input.peek() == -1) {
                        throw ParsingError("Failed, expected \"false\""s);
                    }
                    char c;
                    input.get(c);
                    false_str.push_back(c);
                }
                if (false_str == "false"s) {
                    return Node(false);
                }
                else {
                    throw ParsingError("Failed to convert "s + false_str + " to false"s);
                }
            }
            else {
                input.putback(c);
                auto val = LoadNumber(input);
                return holds_alternative<int>(val) ? Node(get<int>(val)) : Node(get<double>(val));
            }
        }
    } // namespace

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(NodeValueVisitor{ output }, doc.GetRoot().GetValue());
    }


    void NodeValueVisitor::operator()(std::nullptr_t) {
        out << "null"s;
    }
    int NodeValueVisitor::otstup = 0;
    void NodeValueVisitor::operator()(Array array) {
        bool b = false;
        for (int i = 0; i < otstup; ++i) {
            out << "    "s;
        }
        out << "["s << endl;
        ++otstup;
        for (auto node : array) {
            if (b) {
                out << ","s << endl;
            }
            for (int i = 0; i < otstup; ++i) {
                out << "    "s;
            }
            Print(Document{ node }, out);
            b = true;
        }
        --otstup;
        out << endl;
        for (int i = 0; i < otstup; ++i) {
            out << "    "s;
        }
        out << "]"s;
    }
    void NodeValueVisitor::operator()(Dict dict) {
        bool b = false;
        for (int i = 0; i < otstup; ++i) {
            out << "    "s;
        }
        out << "{"s << endl;
        ++otstup;
        for (auto node : dict) {
            if (b) {
                out << ","s << endl;
            }
            for (int i = 0; i < otstup; ++i) {
                out << "    "s;
            }
            Print(Document{ node.first }, out);
            out << ": "s;
            Print(Document{ node.second }, out);
            b = true;
        }
        --otstup;
        out << endl;
        for (int i = 0; i < otstup; ++i) {
            out << "    "s;
        }
        out <<"}"s;
    }
    void NodeValueVisitor::operator()(bool b) {
        b ? out << "true"s : out << "false"s;
    }
    void NodeValueVisitor::operator()(int val) {
        out << val;
    }
    void NodeValueVisitor::operator()(double val) {
        out << val;
    }
    void NodeValueVisitor::operator()(std::string str) {
        unordered_map<char, char> escape_symbols = {
        {'\b','b'},
        {'\f','f'},
        {'\n','n'},
        {'\r','r'},
        {'\t','t'},
        {'\v','v'},
        {'\"','\"'} };

        auto str_iter = str.begin();
        while (str_iter != str.end()) {
            if (*str_iter == '\\') {
                str.insert(str_iter, '\\');
                ++str_iter;
            }
            ++str_iter;
        }

        for (std::uint32_t i = 0; i < str.size(); ++i) {
            if (auto symbol = escape_symbols.find(str[i]); symbol != escape_symbols.end()) {
                auto iter = str.begin() + i;
                str.erase(iter);
                iter = str.begin() + i;
                str.insert(iter, '\\');
                ++i;
                iter = str.begin() + i;
                str.insert(iter, symbol->second);
                ++i;
            }
        }
        str.insert(str.begin(), '\"');
        str.push_back('\"');
        out << str;
    }


} // namespace json
#include "json_builder.h"

using namespace std::literals;

namespace json {
	KeyItemContext Builder::Key(std::string key) {
		if (ready_object_) { // questions
			throw std::logic_error("Object is already ready"s);
		}
		if (nodes_stack_.empty()) {
			throw std::logic_error("Metod Key snaruji slovarya"s);
		}
		else {
			if (!nodes_stack_.back()->IsMap()) {
				throw std::logic_error("Metod Key snaruji slovarya"s);
			}
		}
		keys_.push_back(key);
		KeyItemContext key_con{ *this };
		return key_con;
	}

	Builder& Builder::Value(Node::Value value) {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsArray()) {
				Array& ar = const_cast<Array&>(nodes_stack_.back()->AsArray());
				if (std::holds_alternative<nullptr_t>(value)) {
					ar.push_back(Node{ std::move(std::get<nullptr_t>(value)) });
				}
				else if (std::holds_alternative<Array>(value)) {
					ar.push_back(Node{ std::move(std::get<Array>(value)) });
				}
				else if (std::holds_alternative<Dict>(value)) {
					ar.push_back(Node{ std::move(std::get<Dict>(value)) });
				}
				else if (std::holds_alternative<bool>(value)) {
					ar.push_back(Node{ std::move(std::get<bool>(value)) });
				}
				else if (std::holds_alternative<int>(value)) {
					ar.push_back(Node{ std::move(std::get<int>(value)) });
				}
				else if (std::holds_alternative<double>(value)) {
					ar.push_back(Node{ std::move(std::get<double>(value)) });
				}
				else if (std::holds_alternative<std::string>(value)) {
					ar.push_back(Node{ std::move(std::get<std::string>(value)) });
				}
			}
			else if (nodes_stack_.back()->IsMap()) {
				Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsMap());
				if (std::holds_alternative<nullptr_t>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<nullptr_t>(value)) };
				}
				else if (std::holds_alternative<Array>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<Array>(value)) };
				}
				else if (std::holds_alternative<Dict>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<Dict>(value)) };
				}
				else if (std::holds_alternative<bool>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<bool>(value)) };
				}
				else if (std::holds_alternative<int>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<int>(value)) };
				}
				else if (std::holds_alternative<double>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<double>(value)) };
				}
				else if (std::holds_alternative<std::string>(value)) {
					dict[keys_.back()] = Node{ std::move(std::get<std::string>(value)) };
				}
				keys_.pop_back();
			}
		}
		else {
			if (std::holds_alternative<nullptr_t>(value)) {
				root_ = Node(std::move(std::get<nullptr_t>(value)));
			}
			else if (std::holds_alternative<Array>(value)) {
				root_ =  Node(std::move(std::get<Array>(value)));
			}
			else if (std::holds_alternative<Dict>(value)) {
				root_ =  Node(std::move(std::get<Dict>(value)));
			}
			else if (std::holds_alternative<bool>(value)) {
				root_ = Node(std::move(std::get<bool>(value)));
			}
		    else if (std::holds_alternative<int>(value)) {
				root_ = Node(std::move(std::get<int>(value)));
			}
			else if (std::holds_alternative<double>(value)) {
				root_ = Node(std::move(std::get<double>(value)));
			}
			else if (std::holds_alternative<std::string>(value)) {
				root_ = Node(std::move(std::get<std::string>(value)));
			}
			ready_object_ = true;
		}
		return *this;
	}

	DictItemContext Builder::StartDict() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		Dict dict;
		nodes_stack_.push_back(new Node{ dict });
		DictItemContext dictan{ *this };
		return dictan;
	}

	ArrayItemContext Builder::StartArray() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		Array vec;
		nodes_stack_.push_back(new Node{ vec });
		ArrayItemContext ar{ *this };
		return ar;
	}

	Builder& Builder::EndDict() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		if (nodes_stack_.empty()) {
			throw std::logic_error("Not the End of Dict"s);
		}
		else if (!nodes_stack_.back()->IsMap()) {
			throw std::logic_error("Not the End of Dict"s);
		}
		Node* node = nodes_stack_.back();
		nodes_stack_.pop_back();
		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsArray()) {
				Array& ar = const_cast<Array&>(nodes_stack_.back()->AsArray());
				ar.push_back(*node);
			}
			else if (nodes_stack_.back()->IsMap()) {
				Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsMap());
				dict[keys_.back()] = *node;
			}
		}
		else {
			ready_object_ = true;
		}
		root_ = *node;
		delete node;
		return *this;
	}

	Builder& Builder::EndArray() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		if (nodes_stack_.empty()) {
			throw std::logic_error("Not the End of Dict"s);
		}
		else if (!nodes_stack_.back()->IsArray()) {
			throw std::logic_error("Not the End of Dict"s);
		}
		Node* node = nodes_stack_.back();
		nodes_stack_.pop_back();
		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsArray()) {
				Array& ar = const_cast<Array&>(nodes_stack_.back()->AsArray());
				ar.push_back(*node);
			}
			else if (nodes_stack_.back()->IsMap()) {
				Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsMap());
				dict[keys_.back()] = *node;
			}
		}
		else {
			ready_object_ = true;
		}
		root_ = *node;
		delete node;
		return *this;
	}
	json::Node Builder::Build() {
		if (!ready_object_) {
			throw std::logic_error("Object is NOT ready"s);
		}
		return root_;
	}

	KeyItemContext ValueKeyContext::Key(std::string str) {
		return builder_.Key(str);
	}

	Builder& ValueKeyContext::EndDict() {
		return builder_.EndDict();
	}

	ValueKeyContext KeyItemContext::Value(Node::Value val) {
		builder_.Value(val);
		ValueKeyContext val_con{ builder_ };
		return val_con;
	}

	DictItemContext KeyItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext KeyItemContext::StartArray() {
		return builder_.StartArray();
	}

	KeyItemContext DictItemContext::Key(std::string str) {
		return builder_.Key(str);
	}

	Builder& DictItemContext::EndDict() {
		builder_.EndDict();
		return builder_;
	}

	ArrayItemContext& ArrayItemContext::Value(Node::Value val) {
		builder_.Value(val);
		return *this;
	}

	DictItemContext ArrayItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext& ArrayItemContext::StartArray() {
		builder_.StartArray();
		return *this;
	}

	Builder& ArrayItemContext::EndArray() {
		builder_.EndArray();
		return builder_;
	}
}// namespace json
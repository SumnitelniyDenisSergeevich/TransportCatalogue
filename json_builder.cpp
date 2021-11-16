#include "json_builder.h"

using namespace std::literals;

namespace json {
	KeyItemContext Builder::Key(std::string key) {
		if (ready_object_) {
			throw std::logic_error("the object is already ready"s);
		}
		if (nodes_stack_.empty()) {
			throw std::logic_error("the Key method outside the dictionary"s);
		}
		else {
			if (!nodes_stack_.back()->IsDict()) {
				throw std::logic_error("the Key method outside the dictionary"s);
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
				ar.push_back(Node{ std::move(value) });
			}
			else if (nodes_stack_.back()->IsDict()) {
				Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsMap());
				dict[keys_.back()] = Node{ std::move(value) };
				keys_.pop_back();
			}
		}
		else {
			root_ = Node{ std::move(value) };
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
		else if (!nodes_stack_.back()->IsDict()) {
			throw std::logic_error("Not the End of Dict"s);
		}
		Node* node = nodes_stack_.back();
		nodes_stack_.pop_back();
		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsArray()) {
				Array& ar = const_cast<Array&>(nodes_stack_.back()->AsArray());
				ar.push_back(*node);
			}
			else if (nodes_stack_.back()->IsDict()) {
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
			else if (nodes_stack_.back()->IsDict()) {
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

	ArrayItemContext& ArrayItemContext::Value(Node::Value val) {
		builder_.Value(val);
		return *this;
	}

	ValueKeyContext KeyItemContext::Value(Node::Value val) {
		builder_.Value(val);
		ValueKeyContext val_con{ builder_ };
		return val_con;
	}

	DictItemContext BaseContext::StartDict() {
		return builder_.StartDict();
	}
	ArrayItemContext BaseContext::StartArray() {
		return builder_.StartArray();
	}
	Builder& BaseContext::EndArray() {
		builder_.EndArray();
		return builder_;
	}
	KeyItemContext BaseContext::Key(std::string str) {
		return builder_.Key(str);
	}
	Builder& BaseContext::EndDict() {
		builder_.EndDict();
		return builder_;
	}
}// namespace json
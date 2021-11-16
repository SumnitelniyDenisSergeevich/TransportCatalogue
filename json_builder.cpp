#include "json_builder.h"

using namespace std::literals;

namespace json {
	Builder::KeyItemContext Builder::Key(std::string key) {
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
		Node node;
		if (!nodes_stack_.empty()) {
			if (nodes_stack_.back()->IsArray()) {
				Array& ar = const_cast<Array&>(nodes_stack_.back()->AsArray());
				*node = value;
				ar.push_back(std::move(node));
			}
			else if (nodes_stack_.back()->IsDict()) {
				Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsMap());
				*node = value;
				dict[keys_.back()] = std::move(node);
				keys_.pop_back();
			}
		}
		else {
			*node = value;
			root_ = std::move(node);
			ready_object_ = true;
		}
		return *this;
	}

	Builder::DictItemContext Builder::StartDict() {
		return StartContainer<DictItemContext, Dict>();
	}

	Builder::ArrayItemContext Builder::StartArray() {
		return StartContainer<ArrayItemContext, Array>();
	}

	Builder& Builder::EndDict() {
		return EndContainer<Dict>();
	}

	Builder& Builder::EndArray() {
		return EndContainer<Array>();
	}

	json::Node Builder::Build() {
		if (!ready_object_) {
			throw std::logic_error("Object is NOT ready"s);
		}
		return root_;
	}

	Builder::KeyItemContext  Builder::ValueKeyContext::Key(std::string str) {
		return builder_.Key(str);
	}

	Builder& Builder::ValueKeyContext::EndDict() {
		return builder_.EndDict();
	}

	Builder::ArrayItemContext& Builder::ArrayItemContext::Value(Node::Value val) {
		builder_.Value(val);
		return *this;
	}

	Builder::ValueKeyContext Builder::KeyItemContext::Value(Node::Value val) {
		builder_.Value(val);
		ValueKeyContext val_con{ builder_ };
		return val_con;
	}

	Builder::DictItemContext Builder::BaseContext::StartDict() {
		return builder_.StartDict();
	}

	Builder::ArrayItemContext Builder::BaseContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& Builder::BaseContext::EndArray() {
		builder_.EndArray();
		return builder_;
	}

	Builder::KeyItemContext Builder::BaseContext::Key(std::string str) {
		return builder_.Key(str);
	}

	Builder& Builder::BaseContext::EndDict() {
		builder_.EndDict();
		return builder_;
	}
}// namespace json
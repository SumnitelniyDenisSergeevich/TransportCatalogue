#pragma once

#include "json.h"

namespace json {
	class ArrayItemContext;
	class DictItemContext;
	class KeyItemContext;

	class Builder {
	public:
		Builder& Value(Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();
		KeyItemContext Key(std::string);
	private:
		Node root_{ nullptr };
		std::vector<Node*> nodes_stack_;
		std::vector<std::string> keys_;
		bool ready_object_ = false;
	};

	class ValueKeyContext {// value следующий за key
	public:
		ValueKeyContext(Builder& build) : builder_(build) {
		}
		KeyItemContext Key(std::string str);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

	class KeyItemContext {
	public:
		KeyItemContext(Builder& build) : builder_(build) {
		}
		ValueKeyContext Value(Node::Value val);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
	private:
		Builder& builder_;
	};

	class DictItemContext {
	public:
		DictItemContext(Builder& build) : builder_(build) {
		}
		KeyItemContext Key(std::string str);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

	class ArrayItemContext {
	public:
		ArrayItemContext(Builder& build) : builder_(build) {
		}
		ArrayItemContext& Value(Node::Value val);
		DictItemContext StartDict();
		ArrayItemContext& StartArray();
		Builder& EndArray();
	private:
		Builder& builder_;
	};
}// namespace json
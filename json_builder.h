#pragma once

#include "json.h"

namespace json {
	class Builder {
	private:
		class ArrayItemContext;
		class DictItemContext;
		class KeyItemContext;
	public:
		Builder& Value(Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();
		KeyItemContext Key(std::string);
	private:

		class BaseContext {
		public:
			BaseContext(Builder& build) : builder_(build) {
			}
			DictItemContext StartDict();
			ArrayItemContext StartArray();
			Builder& EndArray();
			KeyItemContext Key(std::string str);
			Builder& EndDict();
		private:
			Builder& builder_;
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

		class KeyItemContext : public BaseContext {
		public:
			KeyItemContext(Builder& build) : BaseContext(build), builder_(build) {
			}
			ValueKeyContext Value(Node::Value val);

			Builder& EndArray() = delete;
			KeyItemContext Key(std::string str) = delete;
			Builder& EndDict() = delete;
		private:
			Builder& builder_;
		};

		class DictItemContext : public BaseContext {
		public:
			DictItemContext StartDict() = delete;
			ArrayItemContext& StartArray() = delete;
			Builder& EndArray() = delete;
		};

		class ArrayItemContext : public BaseContext {
		public:
			ArrayItemContext(Builder& build) : BaseContext(build), builder_(build) {
			}
			ArrayItemContext& Value(Node::Value val);

			KeyItemContext Key(std::string str) = delete;
			Builder& EndDict() = delete;
		private:
			Builder& builder_;
		};

		Node root_{ nullptr };
		std::vector<Node*> nodes_stack_;
		std::vector<std::string> keys_;
		bool ready_object_ = false;
	};
}// namespace json
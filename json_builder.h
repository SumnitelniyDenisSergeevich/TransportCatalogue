#pragma once

#include "json.h"
#include <type_traits>

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
		template<typename ItemContext, typename ContainerType>
		ItemContext StartContainer();
		template<typename ContainerType>
		Builder& EndContainer();

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

	template<typename ItemContext, typename ContainerType>
	ItemContext Builder::StartContainer() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		ContainerType container;
		nodes_stack_.push_back(new Node{ container });
		ItemContext item_context{ *this };
		return item_context;
	}

	template<typename ContainerType>
	Builder& Builder::EndContainer() {
		if (ready_object_) {
			throw std::logic_error("Object is already ready"s);
		}
		if (nodes_stack_.empty()) {
			throw std::logic_error("Not the end of container"s);
		}
		if constexpr (std::is_same_v< ContainerType, Dict>) {
			if (!nodes_stack_.back()->IsDict()) {
				throw std::logic_error("Not the end of Dict"s);
			}
		}else if constexpr (std::is_same_v< ContainerType, Array>) {
			if (!nodes_stack_.back()->IsArray()) {
				throw std::logic_error("Not the end of Array"s);
			}
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

}// namespace json
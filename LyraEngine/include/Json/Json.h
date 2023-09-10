/**************************
 * @file Json.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A JSON parser and writer
 * 
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2022
 **************************/

#pragma once

#include <Common/Common.h>
#include <Common/Logger.h>
#include <Common/UniquePointer.h>
#include <Common/Node.h>
#include <Common/FileSystem.h>

#include <vector>
#include <unordered_map>
#include <variant>

namespace lyra {

template <
	class Literal = char, 
	template <class...> class ArrayContainer = std::vector,
	class Integer = int32,
	class Unsigned = uint32,
	class Floating = float32,
	class HashOrCompare = std::hash<std::basic_string<Literal>>, 
	template <class...> class NodeContainer = std::unordered_map> 
class JsonNode : public Node<JsonNode<Literal, ArrayContainer, Integer, Unsigned, Floating, HashOrCompare, NodeContainer>, std::basic_string<Literal>, HashOrCompare, NodeContainer> {
public:
	struct NullType { };

	using null_type = NullType;
	using literal_type = Literal;
	using integer_type = Integer;
	using unsigned_type = Unsigned;
	using floating_type = Floating;

	using json_type = JsonNode;
	using pointer = json_type*;
	using reference = json_type&;
	using const_reference = const json_type&;

	using string_type = std::basic_string<literal_type>;
	using array_type = ArrayContainer<pointer>;

	using unique_json = UniquePointer<json_type>;
	using node_type = lyra::Node<json_type, string_type, HashOrCompare, NodeContainer>;
	using container_type = std::vector<unique_json>;
	using value_type = std::variant<
		null_type,
		pointer,
		array_type,
		string_type,
		floating_type,
		unsigned_type,
		integer_type,
		bool
	>;

	constexpr JsonNode() noexcept : node_type(this, "") { }
	constexpr JsonNode(value_type&& value) noexcept : node_type(this, ""), m_value(std::move(value)) { }

	template <class Iterator> NODISCARD static constexpr JsonNode parse(Iterator begin, Iterator end) {
		ASSERT(*begin != '{' || *begin != '[', "lyra::Json::parse(): invalid begin of file!");

		// first node
		JsonNode json;
		json.m_self = &json;

		// start parsing
		if (*begin++ == '{') {
			json.m_value = parseObject(begin, end, json);
		}
		else {
			json.m_value = parseArray(begin, end, json);
		}

		return json;
	}
	template <class Container> NODISCARD static constexpr JsonNode parse(const Container& container) {
		return parse(container.begin(), container.end());
	}

	constexpr bool isObject() const noexcept {
		return std::holds_alternative<pointer>(m_value);
	}
	constexpr bool isArray() const noexcept {
		return std::holds_alternative<array_type>(m_value);
	}
	constexpr bool isString() const noexcept {
		return std::holds_alternative<string_type>(m_value);
	}
	constexpr bool isSigned() const noexcept {
		return std::holds_alternative<integer_type>(m_value);
	}
	constexpr bool isUnsigned() const noexcept {
		return std::holds_alternative<unsigned_type>(m_value);
	}
	constexpr bool isInteger() const noexcept {
		return isSigned() || isUnsigned();
	}
	constexpr bool isFloating() const noexcept {
		return std::holds_alternative<floating_type>(m_value);
	}
	constexpr bool isNumber() const noexcept {
		return isInteger() || isFloating();
	}
	constexpr bool isBoolean() const noexcept {
		return std::holds_alternative<bool>(m_value);
	}
	constexpr bool isNull() const noexcept {
		return std::holds_alternative<null_type>(m_value);
	}

	constexpr string_type stringify() const {
		string_type r;

		if (isObject()) stringifyObject(0, *this, r);
		else if (isArray()) stringifyArray(0, *this, r);
		else stringifyPair(0, *this, r);

		return r;
	}

	template <class Ty> constexpr const Ty& get() const noexcept {
		return std::get<Ty>(m_value);
	}
	template <class Ty> constexpr Ty& get() noexcept {
		return std::get<Ty>(m_value);
	}

	template <> constexpr const json_type& get<json_type>() const noexcept {
		return *std::get<pointer>(m_value);
	}
	template <> constexpr json_type& get<json_type>() noexcept {
		return *std::get<pointer>(m_value);
	}

	typename node_type::iterator insert(const json_type& json) {
		m_nodes.emplace_back(json);
		return insert_child(&json).first;
	}
	typename node_type::iterator insert(json_type&& json) {
		m_nodes.emplace_back(std::move(json));
		return insert_child(&json).first;
	}
	typename node_type::iterator insert(const typename node_type::key_type& name, value_type&& value) {
		m_nodes.emplace_back(unique_json::create(std::move(value)));
		m_nodes.back()->m_name = name;
		return this->insert_child(m_nodes.back().get()).first;
	}
	pointer insert(value_type&& value) {
		return m_nodes.emplace_back(unique_json::create(std::move(value)));
	}

	const_reference at(size_t i) const { return *get<array_type>().at(); }
	reference operator[](size_t i) { return *get<array_type>()[i]; }

	reference operator[](const literal_type* name) { return *this->m_children[name]; }

private:
	container_type m_nodes;

	value_type m_value;

	template <class Iterator> static constexpr int skip_characters(Iterator& begin, Iterator& end) {
		for (; begin != end; begin++) {
			switch (*begin) { // skip whitespaces
				case '\n':
				case '\t':
				case '\r':
				case ' ':
				case '\0':
					break;

				default:
					return *begin;
					break;
			}
		}

		return *begin;
	}

	template <class Iterator> static constexpr string_type parseString(Iterator& begin, Iterator& end) {
		ASSERT(*begin == '\"', "lyra::Json::parseString(): JSON Syntax Error: unexcpected token!");

		string_type r;

		for (++begin; begin != end; begin++) {
			switch (*begin) {
				case '\\':
					r.push_back(*begin);
					r.push_back(*++begin);
					break;

				case '\"':
					++begin;
					return r;
					break;
				
				default:
					r.push_back(*begin);
					break;
			}
		}

		ASSERT(false, "lyra::Json::parseString(): JSON Syntax Error: missing token!");
		return r;
	}
	template <class Iterator> static constexpr value_type parsePrimitive(Iterator& begin, Iterator& end) {
		switch(*begin) {
			default:
				ASSERT(false, "lyra::Json::parsePrimitive(): JSON Syntax Error: unexpected token!");
				break;

			case 't':
				begin += 3;
				return true;

				break;
			case 'f':
				begin += 4;
				return false;
				
				break;
			case 'n':
				begin += 3;
				return value_type();

				break;
			
			case '-':
			case '+':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				string_type r;
				bool isUnsigned = true;
				bool isFloat = false;

				for (; begin != end; begin++) {
					switch (*begin) {
						case '-':
							isUnsigned = false;
							r.push_back(*begin);

							break;

						case '+':
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
						case 'A':
						case 'a':
						case 'B':
						case 'b':
						case 'C':
						case 'c':
						case 'D':
						case 'd':
						case 'E':
						case 'e':
						case 'F':
						case 'f':
							r.push_back(*begin);

							break;

						case '.':
							isFloat = true;
							r.push_back(*begin);

							break;
						case '\n':
							if (isFloat) return std::stof(r);
							else if (isUnsigned) return static_cast<unsigned_type>(std::stoi(r));
							else return std::stoi(r);

							break;
					}
				}

				break;
		}

		ASSERT(false, "lyra::Json::parsePrimitive(): JSON Syntax Error: missing token!");

		return value_type();
	}
	template <class Iterator> static constexpr pointer parseObject(Iterator& begin, Iterator& end, json_type& json) {
		for (++begin; begin != end; begin++) {
			switch(skip_characters(begin, end)) {
				case '}':
					return &json;
					break;
				case ',':
					break;

				default:
					json.insert_child(
						json.m_nodes.emplace_back(unique_json::create(parsePair(begin, end, json)).release()).get()
					);
			}
		}

		ASSERT(false, "lyra::Json::parseObject(): JSON Syntax Error: missing token!");

		return &json;
	}
	template <class Iterator> static constexpr array_type parseArray(Iterator& begin, Iterator& end, json_type& json) {
		array_type r;

		for (++begin; begin != end; begin++) {
			json_type tok;
			tok.m_self = &tok;

			switch(skip_characters(begin, end)) {
				case '{':
					tok.m_value = parseObject(++begin, end, tok);

					break;
				case '[':
					tok.m_value = parseArray(++begin, end, tok);

					break;
				case '\"':
					tok.m_value = parseString(begin, end);

					break;
				case ']':
					return r;
					break;
				case ',':
					break;

				default:
					tok.m_value = parsePrimitive(begin, end);
					break;
			}

			r.push_back(
				json.m_nodes.emplace_back(unique_json::create(std::move(tok))).get()
				);
		}

		ASSERT(false, "lyra::Json::parseArray(): JSON Syntax Error: missing token!");

		return r;
	}
	template <class Iterator> static constexpr json_type parsePair(Iterator& begin, Iterator& end, json_type& json) {
		json_type tok;
		tok.m_self = &tok;

		tok.m_name = parseString(begin, end);
		ASSERT(*begin++ == ':', "lyra::Json::parsePair(): JSON Syntax Error: unexcpected token!");

		switch(skip_characters(begin, end)) {
			case '{':
				tok.m_value = parseObject(++begin, end, tok);

				break;
			case '[':
				tok.m_value = parseArray(++begin, end, tok);

				break;
			case '\"':
				tok.m_value = parseString(begin, end);

				break;
			
			case '}':
			case ']':
				++begin;
				break;

			default:
				tok.m_value = parsePrimitive(begin, end);
				break;
		}

		return tok;
	}

	static constexpr void stringifyPrimitive(const json_type& t, string_type& s) {
		if (t.isBoolean()) {
			if (t == true) s.append("true");
			else s.append("false");
		} else if (t.isUnsigned()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<unsigned_type>()));
			else
				s.append(std::to_wstring(t.get<unsigned_type>()));
		} else if (t.isSigned()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<integer_type>()));
			else
				s.append(std::to_wstring(t.get<integer_type>()));
		} else if (t.isFloating()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<float>()));
			else
				s.append(std::to_wstring(t.get<float>()));
		} else 
			s.append("null");
	}
	static constexpr void stringifyObject(size_t indent, const json_type& t, string_type& s) {
		indent++;
		s.append("{\n");
		for (auto it = t.begin(); it != t.end(); it++) {
			if (it != t.begin()) s.append(",\n");
			stringifyPair(indent, *it->second, s);
		}
		indent--;
		s.append("\n").append(indent, '\t').push_back('}');
	}
	static constexpr void stringifyArray(size_t indent, const json_type& t, string_type& s) {
		indent += 1;
		s.append("[\n");
		const auto& array = t.get<array_type>();
		for (auto it = array.rbegin(); it != array.rend(); it++) {
			s.append(indent, '\t');
			if ((*it)->isString())
				s.append("\"").append((*it)->template get<string_type>()).append("\"");
			else if ((*it)->isObject())
				stringifyObject(indent, **it, s);
			else if ((*it)->isArray())
				stringifyArray(indent, **it, s);	
			else
				stringifyPrimitive(**it, s);

			s.append(",\n");
		}
		indent--;
		s.erase(s.size() - 2, 1);
		s.append(indent, '\t').push_back(']');
	}
	static constexpr void stringifyPair(size_t indent, const json_type& t, string_type& s) {
		s.append(indent, '\t').append("\"").append(t.m_name).append("\": ");
		
		if (t.isString())
			s.append("\"").append(t.get<string_type>()).append("\"");
		else if (t.isObject())
			stringifyObject(indent, t, s);
		else if (t.isArray())
			stringifyArray(indent, t, s);	
		else
			stringifyPrimitive(t, s);
	}

	friend class Node<json_type, string_type, HashOrCompare, NodeContainer>;
};

using Json = JsonNode<>;

} // namespace lyra

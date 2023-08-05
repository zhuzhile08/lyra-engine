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
#include <Common/UniquePointer.h>
#include <Common/Node.h>
#include <Common/FileSystem.h>

#include <vector>
#include <unordered_map>
#include <variant>

namespace lyra {

template <
	class Literal = char, 
	class Integer = int32,
	class Unsigned = uint32,
	class Floating = float32,
	class HashOrCompare = std::hash<std::basic_string<Literal>>, 
	template <class...> class NodeContainer = std::unordered_map> 
class JsonNode : lyra::Node<JsonNode<Literal, Integer, Unsigned, Floating, HashOrCompare, NodeContainer>, std::basic_string<Literal>, HashOrCompare, NodeContainer> {
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
	using array_type = std::vector<pointer>;
	using unique_string = UniquePointer<string_type>;
	using unique_array = UniquePointer<array_type>;

	using unique_json = UniquePointer<json_type>;
	using node_type = lyra::Node<json_type, string_type, HashOrCompare, NodeContainer>;
	using container_type = std::vector<unique_json>;
	using value_type = std::variant<
		null_type,
		pointer,
		unique_array,
		unique_string,
		floating_type,
		unsigned_type,
		integer_type,
		bool
	>;

	friend class Node<json_type, string_type, HashOrCompare, NodeContainer>;

public:
	template <class Iterator> NODISCARD static JsonNode parse(Iterator begin, Iterator end) {
		ASSERT(*begin != '{' || *begin != '[', "lyra::Json::parse(): invalid begin of file!");

		// first node
		JsonNode json;
		json.m_self = &json;

		// start parsing
		if (*begin == '{') {
			json.m_value = parse_object(begin, end, json);
		}
		else {
			json.get<unique_array>() = std::move(unique_array::create(parse_array(begin, end, json)));
		}

		return json;
	}
	template <class Container> NODISCARD static JsonNode parse(const Container& container) {
		return parse(container.begin(), container.end());
	}

	bool is_object() const noexcept {
		return std::holds_alternative<pointer>(m_value);
	}
	bool is_array() const noexcept {
		return std::holds_alternative<unique_array>(m_value);
	}
	bool is_string() const noexcept {
		return std::holds_alternative<unique_string>(m_value);
	}
	bool is_signed() const noexcept {
		return std::holds_alternative<integer_type>(m_value);
	}
	bool is_unsigned() const noexcept {
		return std::holds_alternative<unsigned_type>(m_value);
	}
	bool is_integer() const noexcept {
		return is_signed() || is_unsigned();
	}
	bool is_floating() const noexcept {
		return std::holds_alternative<floating_type>(m_value);
	}
	bool is_number() const noexcept {
		return is_integer() || is_floating();
	}
	bool is_boolean() const noexcept {
		return std::holds_alternative<bool>(m_value);
	}
	bool is_null() const noexcept {
		return std::holds_alternative<null_type>(m_value);
	}

	string_type stringify() const {
		string_type r;

		if (is_object()) stringify_object(0, *this, r);
		else if (is_array()) stringify_array(0, *this, r);
		else stringify_pair(0, *this, r);

		return r;
	}

	template <class Ty> constexpr Ty get() const noexcept {
		return std::get<Ty>(m_value);
	}
	template <class Ty> constexpr Ty& get() noexcept {
		return std::get<Ty>(m_value);
	}

	template <> constexpr json_type get<json_type>() const noexcept {
		return *std::get<pointer>(m_value);
	}
	template <> constexpr json_type& get<json_type>() noexcept {
		return *std::get<pointer>(m_value);
	}

	template <> constexpr string_type get() const noexcept {
		return *std::get<unique_string>(m_value).get();
	}
	template <> constexpr string_type& get<string_type>() noexcept {
		return *std::get<unique_string>(m_value);
	}

	template <> constexpr array_type get<array_type>() const noexcept {
		return *std::get<unique_array>(m_value).get();
	}
	template <> constexpr array_type& get<array_type>() noexcept {
		return *std::get<unique_array>(m_value);
	}

	const_reference at(size_t i) const { return *get<array_type>().at(); }
	reference operator[](size_t i) { return *get<array_type>()[i]; }

	reference operator[](const literal_type* name) { return *this->m_children[name]; }

private:
	container_type m_nodes;

	value_type m_value;

	template <class Iterator> static int skip_characters(Iterator& begin, Iterator& end) {
		for (; begin != end; begin++) {
			switch (*begin) { // skip whitespaces
				case '\n':
				case '\t':
				case '\r':
				case ' ':
					break;

				default:
					return *begin;
					break;
			}
		}

		return *begin;
	}

	template <class Iterator> static string_type parse_string(Iterator& begin, Iterator& end) {
		ASSERT(*begin == '\"', "lyra::Json::parse_string(): JSON Syntax Error: unexcpected token!");

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

		ASSERT(false, "lyra::Json::parse_string(): JSON Syntax Error: missing token!");
		return r;
	}
	template <class Iterator> static value_type parse_primitive(Iterator& begin, Iterator& end) {
		switch(*begin) {
			default:
				ASSERT(false, "lyra::Json::parse_primitive(): JSON Syntax Error: unexpected token!");
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

		ASSERT(false, "lyra::Json::parse_primitive(): JSON Syntax Error: missing token!");

		return value_type();
	}
	template <class Iterator> static pointer parse_object(Iterator& begin, Iterator& end, json_type& json) {
		for (++begin; begin != end; begin++) {
			switch(skip_characters(begin, end)) {
				case '}':
					++begin;
					return &json;
					break;
				case ',':
					break;

				default:
					json.insert_child(
						json.m_nodes.emplace_back(unique_json::create(parse_pair(begin, end, json)).release()).get()
					);
			}
		}

		ASSERT(false, "lyra::Json::parse_object(): JSON Syntax Error: missing token!");

		return &json;
	}
	template <class Iterator> static array_type parse_array(Iterator& begin, Iterator& end, json_type& json) {
		array_type r;

		for (++begin; begin != end; begin++) {
			json_type tok;
			tok.m_self = &tok;

			switch(skip_characters(begin, end)) {
				case '{':
					tok.m_value = parse_object(begin, end, tok);

					break;
				case '[':
					tok.m_value = std::move(unique_array::create(parse_array(begin, end, tok)));

					break;
				case '\"':
					tok.m_value = std::move(unique_string::create(parse_string(begin, end)));

					break;
				case ']':
					++begin;
					return r;
					break;
				case ',':
					break;

				default:
					tok.m_value = parse_primitive(begin, end);
					break;
			}

			r.push_back(
				json.m_nodes.emplace_back(unique_json::create(std::move(tok))).get()
				);
		}

		ASSERT(false, "lyra::Json::parse_array(): JSON Syntax Error: missing token!");

		return r;
	}
	template <class Iterator> static json_type parse_pair(Iterator& begin, Iterator& end, json_type& json) {
		json_type tok;
		tok.m_self = &tok;

		tok.m_name = parse_string(begin, end);
		ASSERT(*begin++ == ':', "lyra::Json::parse_pair(): JSON Syntax Error: unexcpected token!");

		switch(skip_characters(begin, end)) {
			case '{':
				tok.m_value = parse_object(begin, end, tok);

				break;
			case '[':
				tok.m_value = std::move(unique_array::create(parse_array(begin, end, tok)));

				break;
			case '\"':
				tok.m_value = std::move(unique_string::create(parse_string(begin, end)));

				break;

			default:
				tok.m_value = parse_primitive(begin, end);
				break;
		}

		return tok;
	}

	static void stringify_primitive(const json_type& t, string_type& s) {
		if (t.is_boolean()) {
			if (t == true) s.append("true");
			else s.append("false");
		} else if (t.is_unsigned()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<unsigned_type>()));
			else
				s.append(std::to_wstring(t.get<unsigned_type>()));
		} else if (t.is_signed()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<integer_type>()));
			else
				s.append(std::to_wstring(t.get<integer_type>()));
		} else if (t.is_floating()) {
			if constexpr(sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<float>()));
			else
				s.append(std::to_wstring(t.get<float>()));
		} else 
			s.append("null");
	}
	static void stringify_object(size_t indent, const json_type& t, string_type& s) {
		indent++;
		s.append("{\n");
		for (auto it = t.begin(); it != t.end(); it++) {
			stringify_pair(indent, *it->second, s);

			s.append(",\n");
		}
		indent--;
		s.erase(s.size() - 2, 1);
		s.append(indent, '\t').push_back('}');
	}
	static void stringify_array(size_t indent, const json_type& t, string_type& s) {
		indent += 1;
		s.append("[\n");
		for (const auto& it : t.get<array_type>()) {
			s.append(indent, '\t');
			if (it->is_string())
				s.append("\"").append(it->template get<string_type>()).append("\"");
			else if (it->is_object())
				stringify_object(indent, *it, s);
			else if (it->is_array())
				stringify_array(indent, *it, s);	
			else
				stringify_primitive(*it, s);

			s.append(",\n");
		}
		indent--;
		s.erase(s.size() - 2, 1);
		s.append(indent, '\t').push_back(']');
	}
	static void stringify_pair(size_t indent, const json_type& t, string_type& s) {
		s.append(indent, '\t').append("\"").append(t.m_name).append("\": ");
		
		if (t.is_string())
			s.append("\"").append(t.get<string_type>()).append("\"");
		else if (t.is_object())
			stringify_object(indent, t, s);
		else if (t.is_array())
			stringify_array(indent, t, s);	
		else
			stringify_primitive(t, s);
	}
};

using Json = JsonNode<>;

} // namespace lyra

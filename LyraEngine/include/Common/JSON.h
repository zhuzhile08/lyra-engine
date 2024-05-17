/**************************
 * @file JSON.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A JSON parser and writer
 * 
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 **************************/

#pragma once

#include <Common/Common.h>
#include <Common/Logger.h>
#include <Common/UniquePointer.h>
#include <Common/SharedPointer.h>
#include <Common/Node.h>

#include <Common/Vector.h>
#include <Common/UnorderedSparseMap.h>
#include <variant>

namespace lyra {

template <
	class Literal = char, 
	template <class...> class ArrayContainer = Vector,
	class Integer = int32,
	class Unsigned = uint32,
	class Floating = float32,
	template <class...> class NodeContainer = UnorderedSparseSet,
	template <class...> class SmartPointer = UniquePointer> 
class BasicJson : public BasicNode<
	BasicJson<Literal, ArrayContainer, Integer, Unsigned, Floating, NodeContainer, SmartPointer>, 
	SmartPointer,
	std::basic_string<Literal>, 
	NodeContainer> {
public:
	struct NullType { };

	using null_type = NullType;
	using integer_type = Integer;
	using unsigned_type = Unsigned;
	using floating_type = Floating;
	using literal_type = Literal;
	using string_type = std::basic_string<literal_type>;

	using json_type = BasicJson;
	using reference = json_type&;
	using const_reference = const json_type&;
	using rvreference = json_type&&;
	using pointer = json_type*;
	using smart_pointer = SmartPointer<json_type>;

	using array_type = ArrayContainer<smart_pointer>;
	using node_type = BasicNode<json_type, SmartPointer, string_type, NodeContainer>;
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

	constexpr BasicJson() noexcept = default;
	constexpr BasicJson(const value_type& value) noexcept requires std::is_copy_assignable_v<smart_pointer> : m_value(value) { }
	constexpr BasicJson(value_type&& value) noexcept : m_value(std::move(value)) { }
	template <class KeyType> constexpr BasicJson(KeyType&& key, value_type&& value) noexcept : node_type(std::forward<KeyType>(key)), m_value(std::move(value)) { }
	constexpr BasicJson(BasicJson&&) = default;
	constexpr BasicJson(const BasicJson&) requires std::is_copy_assignable_v<smart_pointer> = default;
	constexpr ~BasicJson() noexcept = default;

	constexpr reference operator=(const value_type& value) noexcept requires std::is_copy_assignable_v<smart_pointer> {
		m_value = value;
		return *this;
	}
	constexpr reference operator=(value_type&& value) noexcept {
		m_value = std::move(value);
		return *this;
	}
	template <class Value> constexpr reference operator=(const Value& value) noexcept requires std::is_copy_assignable_v<smart_pointer> { 
		m_value = value;
		return *this;
	}
	template <class Value> constexpr reference operator=(Value&& value) noexcept {
		m_value = std::forward<Value>(value);
		return *this;
	}

	constexpr reference operator=(const_reference other) noexcept requires std::is_copy_assignable_v<smart_pointer> = default;
	constexpr reference operator=(rvreference other) noexcept = default;

	template <class Iterator> NODISCARD static constexpr json_type parse(Iterator begin, Iterator end) {
		// first node
		json_type json;

		skipCharacters(begin, end);

		// start parsing
		if (*begin == '{') {
			json.m_value = parseObject(begin, end, json);
		} else if (*begin == '[') {
			json.m_value = parseArray(begin, end);
		} else if (++begin == end) {
			log::warning("Requested JSON file to parse was empty! JSON node defaults to object type.");
			json.m_value = &json;
		} else {
			ASSERT(false, "lyra::Json::parse(): invalid file!");
		}

		return json;
	}
	template <class Container> NODISCARD static constexpr json_type parse(const Container& container) {
		return parse(container.begin(), container.end());
	}

	template <class... Args> NODISCARD static constexpr smart_pointer create(Args&&... args) {
		return smart_pointer::create(std::forward<Args>(args)...);
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

		if (isObject()) stringifyObject(*this, r);
		else if (isArray()) stringifyArray(*this, r);
		else stringifyPair(*this, r);

		return r;
	}
	constexpr string_type stringifyPretty() const {
		string_type r;

		if (isObject()) stringifyObjectPretty(0, *this, r);
		else if (isArray()) stringifyArrayPretty(0, *this, r);
		else stringifyPairPretty(0, *this, r);

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

	const_reference at(size_type i) const { return *get<array_type>().at(i); }
	reference operator[](size_type i) { return *get<array_type>()[i]; }

	using node_type::operator[];

private:
	value_type m_value;

	template <class Iterator> static constexpr int skipCharacters(Iterator& begin, Iterator& end) {
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
				string_type r { };
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

						default:
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
			switch(skipCharacters(begin, end)) {
				default:
					json.insert(parsePair(begin, end));
					break;
					
				case '}':
					return &json;
					break;
				case ',':
					break;
			}
		}

		ASSERT(false, "lyra::Json::parseObject(): JSON Syntax Error: missing token!");

		return &json;
	}
	template <class Iterator> static constexpr array_type parseArray(Iterator& begin, Iterator& end) {
		array_type r;

		for (++begin; begin != end; begin++) {
			auto tok = smart_pointer::create();

			switch(skipCharacters(begin, end)) {
				case '{':
					tok->m_value = parseObject(begin, end, *tok);
					r.emplaceBack(tok.release());

					break;
				case '[':
					tok->m_value = parseArray(begin, end);
					r.emplaceBack(tok.release());

					break;
				case '\"':
					tok->m_value = parseString(begin, end);
					r.emplaceBack(tok.release());

					if (*(begin + 1) != ']') ++begin;

					break;
				case ']':
					return r;
					break;
				case '}':
				case ',':
					break;

				default:
					tok->m_value = parsePrimitive(begin, end);
					r.emplaceBack(tok.release());
					
					break;
			}
		}

		ASSERT(false, "lyra::Json::parseArray(): JSON Syntax Error: missing token!");

		return r;
	}
	template <class Iterator> static constexpr json_type parsePair(Iterator& begin, Iterator& end) {
		json_type tok;

		tok.m_name = parseString(begin, end);
		++begin;
		ASSERT(skipCharacters(begin, end) == ':', "lyra::Json::parsePair(): JSON Syntax Error: unexcpected token!");
		++begin;

		switch(skipCharacters(begin, end)) {
			case '{':
				tok.m_value = parseObject(begin, end, tok);

				break;
			case '[':
				tok.m_value = parseArray(begin, end);

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
			if constexpr (sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<unsigned_type>()));
			else
				s.append(std::to_wstring(t.get<unsigned_type>()));
		} else if (t.isSigned()) {
			if constexpr (sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<integer_type>()));
			else
				s.append(std::to_wstring(t.get<integer_type>()));
		} else if (t.isFloating()) {
			if constexpr (sizeof(literal_type) <= 1)
				s.append(std::to_string(t.get<float32>()));
			else
				s.append(std::to_wstring(t.get<float32>()));
		} else 
			s.append("null");
	}
	static constexpr void stringifyObject(const json_type& t, string_type& s) {
		s.push_back('{');
		for (auto it = t.begin(); it != t.end(); it++) {
			if (it != t.begin()) s.push_back(',');
			stringifyPair(*dynamic_cast<json_type*>(it->get()), s);
		}
		s.push_back('}');
	}
	static constexpr void stringifyArray(const json_type& t, string_type& s) {
		s.push_back('[');
		const auto& array = t.get<array_type>();
		for (auto it = array.rbegin(); it != array.rend(); it++) {
			if (it != array.rbegin()) s.push_back(',');
			if ((*it)->isString())
				s.append("\"").append((*it)->template get<string_type>()).push_back('\"');
			else if ((*it)->isObject())
				stringifyObject(**it, s);
			else if ((*it)->isArray())
				stringifyArray(**it, s);	
			else
				stringifyPrimitive(**it, s);
		}
		s.push_back(']');
	}
	static constexpr void stringifyPair(const json_type& t, string_type& s) {
		s.append("\"").append(t.m_name).append("\":");
		
		if (t.isString())
			s.append("\"").append(t.get<string_type>()).append("\"");
		else if (t.isObject())
			stringifyObject(t, s);
		else if (t.isArray())
			stringifyArray(t, s);	
		else
			stringifyPrimitive(t, s);
	}

	static constexpr void stringifyObjectPretty(size_type indent, const json_type& t, string_type& s) {
		indent++;
		s.append("{\n");
		for (auto it = t.begin(); it != t.end(); it++) {
			if (it != t.begin()) s.append(",\n");
			stringifyPairPretty(indent, *it->get(), s);
		}
		s.append("\n").append(--indent, '\t').push_back('}');
	}
	static constexpr void stringifyArrayPretty(size_type indent, const json_type& t, string_type& s) {
		indent += 1;
		s.append("[\n");
		const auto& array = t.get<array_type>();
		for (auto it = array.begin(); it != array.end(); it++) {
			if (it != array.begin()) s.append(",\n");
			s.append(indent, '\t');

			if ((*it)->isString())
				s.append("\"").append((*it)->template get<string_type>()).push_back('\"');
			else if ((*it)->isObject())
				stringifyObjectPretty(indent, **it, s);
			else if ((*it)->isArray())
				stringifyArrayPretty(indent, **it, s);	
			else
				stringifyPrimitive(**it, s);
		}
		s.append("\n").append(--indent, '\t').push_back(']');
	}
	static constexpr void stringifyPairPretty(size_type indent, const json_type& t, string_type& s) {
		s.append(indent, '\t').append("\"").append(t.m_name).append("\": ");
		
		if (t.isString())
			s.append("\"").append(t.get<string_type>()).push_back('\"');
		else if (t.isObject())
			stringifyObjectPretty(indent, t, s);
		else if (t.isArray())
			stringifyArrayPretty(indent, t, s);	
		else
			stringifyPrimitive(t, s);
	}
};

using Json = BasicJson<>;
using SharedJson = BasicJson<char, Vector, int32, uint32, float32, UnorderedSparseMap, SharedPointer>;
using WJson = BasicJson<wchar>;
using WSharedJson = BasicJson<wchar, Vector, int32, uint32, float32, UnorderedSparseMap, SharedPointer>;

} // namespace lyra

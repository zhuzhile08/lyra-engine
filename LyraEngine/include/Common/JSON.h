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

#include <LSD/UniquePointer.h>
#include <LSD/SharedPointer.h>
#include <LSD/Node.h>
#include <LSD/Vector.h>
#include <LSD/UnorderedSparseMap.h>

#include <exception>
#include <variant>

namespace lyra {

class JsonParseError : public std::runtime_error {
public:
	JsonParseError(const lsd::String& message) : std::runtime_error(message.cStr()) {
		m_message.append(message).pushBack('!');
	}
	JsonParseError(const char* message) : std::runtime_error(message) {
		m_message.append(message).pushBack('!');
	}
	JsonParseError(const JsonParseError&) = default;
	JsonParseError(JsonParseError&&) = default;

	JsonParseError& operator=(const JsonParseError&) = default;
	JsonParseError& operator=(JsonParseError&&) = default;

	const char* what() const noexcept override {
		return m_message.cStr();
	}

private:
	lsd::String m_message { "Program terminated with lyra::JsonParseError: " };
};

template <
	class Literal = char, 
	template <class...> class ArrayContainer = lsd::Vector,
	class Integer = int32,
	class Unsigned = uint32,
	class Floating = float32,
	template <class...> class NodeContainer = lsd::UnorderedSparseSet,
	template <class...> class SmartPointer = lsd::UniquePointer> 
class BasicJson : public lsd::BasicNode<
	BasicJson<Literal, ArrayContainer, Integer, Unsigned, Floating, NodeContainer, SmartPointer>, 
	SmartPointer,
	lsd::BasicString<Literal>, 
	NodeContainer> {
public:
	struct NullType { };

	using null_type = NullType;
	using integer_type = Integer;
	using unsigned_type = Unsigned;
	using floating_type = Floating;
	using literal_type = Literal;
	using string_type = lsd::BasicString<literal_type>;

	using json_type = BasicJson;
	using reference = json_type&;
	using const_reference = const json_type&;
	using rvreference = json_type&&;
	using pointer = json_type*;
	using smart_pointer = SmartPointer<json_type>;

	using array_type = ArrayContainer<smart_pointer>;
	using node_type = lsd::BasicNode<json_type, SmartPointer, string_type, NodeContainer>;
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
			// throw std::exception
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
		for (; begin != end; begin++)
			if (!std::isspace(*begin)) return *begin;

		return *begin;
	}

	template <class Iterator> static constexpr string_type parseString(Iterator& begin, Iterator& end) {
		if (*begin != '\"') throw JsonParseError("lyra::Json::parseString(): JSON Syntax Error: unexcpected token!");

		string_type r;

		for (++begin; begin != end; begin++) {
			switch (*begin) {
				case '\\':
					r.pushBack(*++begin);
					break;

				case '\"':
					return r;
					break;
				
				default:
					r.pushBack(*begin);
					break;
			}
		}

		throw JsonParseError("lyra::Json::parseString(): JSON Syntax Error: missing token!");
		return r;
	}
	template <class Iterator> static constexpr value_type parsePrimitive(Iterator& begin, Iterator& end) {
		switch(*begin) {
			default:
				throw JsonParseError("lyra::Json::parseString(): JSON Syntax Error: unexcpected token!");
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
							r.pushBack(*begin);

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
							r.pushBack(*begin);

							break;

						case '.':
							isFloat = true;
							r.pushBack(*begin);

							break;

						default:
							if (isFloat) return lsd::stof(r);
							else if (isUnsigned) return static_cast<unsigned_type>(lsd::stoi(r));
							else return lsd::stoi(r);

							break;
					}
				}

				break;
		}

		throw JsonParseError("lyra::Json::parsePrimitive(): JSON Syntax Error: missing token!");

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

		throw JsonParseError("lyra::Json::parseObject(): JSON Syntax Error: missing token!");

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

		throw JsonParseError("lyra::Json::parseArray(): JSON Syntax Error: missing token!");

		return r;
	}
	template <class Iterator> static constexpr json_type parsePair(Iterator& begin, Iterator& end) {
		json_type tok;

		tok.m_name = parseString(begin, end);
		++begin;
		if (skipCharacters(begin, end) != ':') throw JsonParseError("lyra::Json::parsePair(): JSON Syntax Error: unexcpected token!");
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
				s.append(lsd::toString(t.get<unsigned_type>()));
			else
				s.append(lsd::toWString(t.get<unsigned_type>()));
		} else if (t.isSigned()) {
			if constexpr (sizeof(literal_type) <= 1)
				s.append(lsd::toString(t.get<integer_type>()));
			else
				s.append(lsd::toWString(t.get<integer_type>()));
		} else if (t.isFloating()) {
			if constexpr (sizeof(literal_type) <= 1)
				s.append(lsd::toString(t.get<float32>()));
			else
				s.append(lsd::toWString(t.get<float32>()));
		} else 
			s.append("null");
	}
	static constexpr void stringifyObject(const json_type& t, string_type& s) {
		s.pushBack('{');
		for (auto it = t.begin(); it != t.end(); it++) {
			if (it != t.begin()) s.pushBack(',');
			stringifyPair(*dynamic_cast<json_type*>(it->get()), s);
		}
		s.pushBack('}');
	}
	static constexpr void stringifyArray(const json_type& t, string_type& s) {
		s.pushBack('[');
		const auto& array = t.get<array_type>();
		for (auto it = array.rbegin(); it != array.rend(); it++) {
			if (it != array.rbegin()) s.pushBack(',');
			if ((*it)->isString())
				s.append("\"").append((*it)->template get<string_type>()).pushBack('\"');
			else if ((*it)->isObject())
				stringifyObject(**it, s);
			else if ((*it)->isArray())
				stringifyArray(**it, s);	
			else
				stringifyPrimitive(**it, s);
		}
		s.pushBack(']');
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
		s.append("\n").append(--indent, '\t').pushBack('}');
	}
	static constexpr void stringifyArrayPretty(size_type indent, const json_type& t, string_type& s) {
		indent += 1;
		s.append("[\n");
		const auto& array = t.get<array_type>();
		for (auto it = array.begin(); it != array.end(); it++) {
			if (it != array.begin()) s.append(",\n");
			s.append(indent, '\t');

			if ((*it)->isString())
				s.append("\"").append((*it)->template get<string_type>()).pushBack('\"');
			else if ((*it)->isObject())
				stringifyObjectPretty(indent, **it, s);
			else if ((*it)->isArray())
				stringifyArrayPretty(indent, **it, s);	
			else
				stringifyPrimitive(**it, s);
		}
		s.append("\n").append(--indent, '\t').pushBack(']');
	}
	static constexpr void stringifyPairPretty(size_type indent, const json_type& t, string_type& s) {
		s.append(indent, '\t').append("\"").append(t.m_name).append("\": ");
		
		if (t.isString())
			s.append("\"").append(t.get<string_type>()).pushBack('\"');
		else if (t.isObject())
			stringifyObjectPretty(indent, t, s);
		else if (t.isArray())
			stringifyArrayPretty(indent, t, s);	
		else
			stringifyPrimitive(t, s);
	}
};

using Json = BasicJson<>;
using SharedJson = BasicJson<char, lsd::Vector, int32, uint32, float32, lsd::UnorderedSparseMap, lsd::SharedPointer>;
using WJson = BasicJson<wchar>;
using WSharedJson = BasicJson<wchar, lsd::Vector, int32, uint32, float32, lsd::UnorderedSparseMap, lsd::SharedPointer>;

} // namespace lyra

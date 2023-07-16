/*************************
 * @file Manager.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A class for manager style systems
 * 
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>

#include <Common/SmartPointer.h>
#include <string>
#include <unordered_map>

namespace lyra {

namespace detail {

template <class KTy> struct DefaultKeyToConstructor {
	using key = KTy;

	constexpr DefaultKeyToConstructor() = default;
	template <class CTy> constexpr DefaultKeyToConstructor(const DefaultKeyToConstructor<CTy>& other) noexcept { }

	constexpr key operator()(const key& k) const noexcept {
		return k;
	}
};

} // namespace detail

template <class KTy, class Ty, class CTy = detail::DefaultKeyToConstructor<Ty>, class ATy = KTy> class Manager {
public:
	using key = KTy;
	using access = ATy;
	using value_type = Ty;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using smart_pointer = SmartPointer<value_type>;
	using constructor = CTy;

	Manager() = default;
	Manager(const constructor& c) : m_constructor(c) { }
	Manager(constructor&& c) : m_constructor(c) { }
	template <class OCTy> Manager(const OCTy& c) : m_constructor(c) { }
	template <class OCTy> Manager(OCTy&& c) : m_constructor(c) { }

	NODISCARD constexpr const const_pointer operator[](access key) {
		if (!m_resources.contains(key)) {
			m_resources.emplace(key, smart_pointer::create(m_constructor(key)));
		}
		return m_resources.at(key);
	}
	NODISCARD constexpr const const_pointer operator[](access key) const {
		return m_resources.at(key);
	}
	NODISCARD constexpr const const_pointer at(access key) const {
		return m_resources.at(key);
	}

private:
	std::unordered_map<key, smart_pointer> m_resources;
	constructor m_constructor;
};

template <class Ty, class CTy> class Manager<std::string, Ty, CTy, std::string_view> {
public:
	using key = std::string;
	using access = std::string_view;
	using value_type = Ty;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using smart_pointer = SmartPointer<value_type>;
	using constructor = CTy;

	Manager() = default;
	Manager(const constructor& c) : m_constructor(c) { }
	Manager(constructor&& c) : m_constructor(c) { }

	NODISCARD constexpr const const_pointer operator[](access key) {
		if (!m_resources.contains(key)) {
			m_resources.emplace(key, smart_pointer::create(m_constructor(key)));
		}
		return m_resources.at(key);
	}
	NODISCARD constexpr const const_pointer operator[](access key) const {
		return m_resources.at(key);
	}
	NODISCARD constexpr const const_pointer at(access key) const {
		return m_resources.at(key);
	}

private:
	std::unordered_map<key, smart_pointer> m_resources;
	constructor m_constructor;
};

} // namespace lyra

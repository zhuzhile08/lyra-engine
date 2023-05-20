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

#include <Lyra/Lyra.h>

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

/**
 * @brief A manager for resources
 * 
 * @tparam KTy key type
 * @tparam Ty value type
 * @tparam CTy key to constructor parameter converter
 * @tparam ATy access key type, used for cases like std::string_view
 */

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
	/**
	 * @brief constuct a new manager class
	 * 
	 * @param c key to constructor converter
	 */
	Manager(const constructor& c) : m_constructor(c) { }
	/**
	 * @brief constuct a new manager class
	 * 
	 * @param c key to constructor converter
	 */
	Manager(constructor&& c) : m_constructor(c) { }
	/**
	 * @brief constuct a new manager class
	 * 
	 * @tparam OCTy a type castable to the constructor type
	 * 
	 * @param c key to constructor converter
	 */
	template <class OCTy> Manager(const OCTy& c) : m_constructor(c) { }
	/**
	 * @brief constuct a new manager class
	 * 
	 * @tparam OCTy a type castable to the constructor type
	 * 
	 * @param c key to constructor converter
	 */
	template <class OCTy> Manager(OCTy&& c) : m_constructor(c) { }

	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer operator[](access key) {
		if (!m_resources.contains(key)) {
			m_resources.emplace(key, smart_pointer::create(m_constructor(key)));
		}
		return m_resources.at(key);
	}
	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer operator[](access key) const {
		return m_resources.at(key);
	}
	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer at(access key) const {
		return m_resources.at(key);
	}

private:
	std::unordered_map<key, smart_pointer> m_resources;
	constructor m_constructor;
};

/**
 * @brief A manager for resources with strings as keys
 * 
 * @tparam Ty type to manage
 * @tparam CTy key to constructor parameter converter
 */
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
	/**
	 * @brief constuct a new manager class
	 * 
	 * @param c key to constructor converter
	 */
	Manager(const constructor& c) : m_constructor(c) { }
	/**
	 * @brief constuct a new manager class
	 * 
	 * @param c key to constructor converter
	 */
	Manager(constructor&& c) : m_constructor(c) { }

	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer operator[](access key) {
		if (!m_resources.contains(key)) {
			m_resources.emplace(key, smart_pointer::create(m_constructor(key)));
		}
		return m_resources.at(key);
	}
	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer operator[](access key) const {
		return m_resources.at(key);
	}
	/**
	 * @brief get a resource based on its key
	 * 
	 * @param key key value
	 * 
	 * @return const Ty* const
	 */
	NODISCARD constexpr const const_pointer at(access key) const {
		return m_resources.at(key);
	}

private:
	std::unordered_map<key, smart_pointer> m_resources;
	constructor m_constructor;
};

} // namespace lyra

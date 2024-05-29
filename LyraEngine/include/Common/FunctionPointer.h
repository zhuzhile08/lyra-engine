/*************************
 * @file FunctionPointer.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a basic wrapper around a C style function pointer
 * 
 * @date 2022-12-05
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/UniquePointer.h>
#include <Common/Utility.h>

#include <utility>

namespace lyra {

template <class> class Function;

template <class Ty, class... Args> class Function <Ty(Args...)> {
private:
	using result = Ty;
	using wrapper = Function;

	class BasicCallableWrapper {
	public:
		virtual ~BasicCallableWrapper() = default;

		virtual constexpr UniquePointer<BasicCallableWrapper> clone() const noexcept = 0;

		virtual constexpr const std::type_info& standardTargetType() const noexcept = 0;
		virtual constexpr type_id targetType() const noexcept = 0;

		virtual constexpr Ty run(Args...) noexcept = 0;
	};

	template <class Callable> class CallableWrapper : public BasicCallableWrapper {
	public:
		using callable_type = Callable;

		CallableWrapper(const callable_type& c) noexcept : m_callable(c) { }
		CallableWrapper(callable_type&& c) noexcept : m_callable(std::move(c)) { }

		constexpr UniquePointer<BasicCallableWrapper> clone() const noexcept override {
			return UniquePointer<CallableWrapper>::create(m_callable);
		}

		constexpr const std::type_info& standardTargetType() const noexcept override {
			return typeid(callable_type);
		}
		constexpr type_id targetType() const noexcept override {
			return typeId<callable_type>();
		}

		constexpr Ty run(Args... args) noexcept override {
			return std::invoke(m_callable, std::forward<Args>(args)...);
		}
	
	private:
		callable_type m_callable;
	};

	using basic_callable = UniquePointer<BasicCallableWrapper>;
	template <class Callable> using real_callable = UniquePointer<CallableWrapper<Callable>>;

public:
	constexpr Function() noexcept = default;
	constexpr Function(const wrapper& function) noexcept : m_callable(function.m_callable ? function.m_callable->clone() : nullptr) { }
	constexpr Function(wrapper&& function) noexcept : m_callable(std::move(function.function)) { }
	template <class Callable> constexpr Function(Callable&& callable) noexcept requires std::is_invocable_r_v<Ty, Callable, Args...> : 
		m_callable(real_callable<Callable>::create(std::forward<Callable>(callable))) { }

	constexpr Function& operator=(const wrapper& function) noexcept {
		m_callable = (function.m_callable) ? std::move(function.m_callable->clone()) : nullptr;
		return *this;
	}
	constexpr Function& operator=(wrapper&& function) noexcept {
		m_callable = std::move(function.m_callable);
		return *this;
	}
	constexpr Function& operator=(std::nullptr_t) noexcept {
		m_callable.reset();
		return *this;
	}
	template <class Callable> constexpr Function& operator=(Callable&& callable) noexcept requires std::is_invocable_r_v<Ty, Callable, Args...> {
		m_callable = real_callable<Callable>::create(std::forward<Callable>(callable));
		return *this;
	}

	constexpr void swap(wrapper& second) noexcept {
		m_callable.swap(second->m_callable);
	}

	DEPRECATED constexpr const std::type_info& standardTargetType() const noexcept {
		if (m_callable) return m_callable->standardTargetType();
		else return typeid(void);
	}
	DEPRECATED constexpr const std::type_info& target_type() const noexcept {
		return targetType();
	}
	constexpr const type_id targetType() const noexcept {
		if (m_callable) return m_callable->targetType();
		else return typeId<void>();
	}

	template<class TTy> constexpr TTy* target() noexcept {
		if constexpr (targetType() == typeId<TTy>()) return m_callable;
		else return nullptr;
	}
	template<class TTy> constexpr const TTy* target() const noexcept{
		if constexpr (targetType() == typeId<TTy>()) return m_callable;
		else return nullptr;
	}

	constexpr operator bool() const noexcept {
		return m_callable;
	}
	constexpr result operator()(Args... args) {
		if (!m_callable) throw std::bad_function_call();
		return m_callable->run(std::forward<Args>(args)...);
	}

private:
	basic_callable m_callable {};
};

} // namespace lyra

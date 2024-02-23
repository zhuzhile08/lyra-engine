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

	class BasicCallable {
	public:
		BasicCallable() = default;
		BasicCallable(const BasicCallable&) = default;
		BasicCallable(BasicCallable&&) = default;
		virtual ~BasicCallable() = default;
		virtual constexpr UniquePointer<BasicCallable> clone() const noexcept = 0;
		virtual constexpr Ty operator()(Args&&...) const noexcept = 0;
	};

	template <class C> class Callable : public BasicCallable {
	public:
		using callable_type = C;
		using pointer = UniquePointer<Callable>;

		template <class OC> Callable(const OC& c) : callable(c) { }
		template <class OC> Callable(OC&& c) : callable(std::forward<OC>(c)) { }

		constexpr UniquePointer<BasicCallable> clone() const noexcept {
			return pointer::create(callable);
		}

		constexpr Ty operator()(Args&&... args) const noexcept {
			return callable(std::forward<Args>(args)...);
		}
	
	private:
		callable_type callable;
	};

	using basic_callable = UniquePointer<BasicCallable>;
	template <class C> using real_callable = UniquePointer<Callable<C>>;

public:
	constexpr Function() noexcept = default;
	constexpr Function(const wrapper& function) noexcept : callable((function.callable) ? function.callable->clone() : nullptr) { }
	constexpr Function(wrapper&& function) noexcept : callable(std::move(function.function)) { }
	template <class Callable> constexpr Function(const Callable& callable) noexcept : 
		callable(real_callable<typename CallableUnderlying<Callable>::type>::create(callable)) { }
	template <class Callable> constexpr Function(Callable&& callable) noexcept : 
		callable(real_callable<typename CallableUnderlying<Callable>::type>::create(std::forward<Callable>(callable))) { }

	constexpr Function& operator=(const wrapper& function) noexcept {
		this->callable = (function.callable) ? function.callable->clone() : nullptr;
	}
	constexpr Function& operator=(wrapper&& function) noexcept {
		this->callable = std::move(function.callable);
	}
	template <class Callable> constexpr Function& operator=(const Callable& callable) noexcept {
		this->callable = real_callable<Callable>::create(callable);
	}
	template <class Callable> constexpr Function& operator=(Callable&& callable) noexcept {
		this->callable = real_callable<Callable>::create(std::forward<Callable>(callable));
	}

	constexpr void swap(wrapper& second) noexcept {
		this->callable.swap(second->callable);
	}

	constexpr const std::type_info& targetType() const noexcept {
		if (!std::is_same_v<Ty, void>) return typeid(Ty);
		return typeid(void);	
	}
	template<class TTy> constexpr TTy* target() noexcept {
		if (targetType() == typeid(TTy)) return callable;
		return nullptr;
	}
	template<class TTy> constexpr const TTy* target() const noexcept{
		if (targetType() == typeid(TTy)) return callable;
		return nullptr;
	}

	constexpr operator bool() const noexcept {
		return static_cast<bool>(callable);
	}
	constexpr result operator()(Args&&... arguments) const {
		return (*callable)(std::forward<Args>(arguments)...);
	}

private:
	basic_callable callable {};
};

} // namespace lyra

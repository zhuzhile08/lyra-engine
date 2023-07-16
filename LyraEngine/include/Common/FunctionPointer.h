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

#include <utility>

namespace lyra {

template <class> class Function;

template <class Ty, class... Args> class Function <Ty(Args...)> {
public:
	using result = Ty;
	using wrapper = Function;
	typedef result(*callable)(Args...); // don't know how to make this with C++ type aliases, but this is good enough anyways

	constexpr Function() noexcept = default;
	constexpr Function(const wrapper& function) noexcept : function(function.function) { }
	constexpr Function(wrapper&& function) noexcept : function(std::move(function.function)) { }
	constexpr Function(const callable& callable) noexcept : function(callable) { }
	constexpr Function(callable&& callable) noexcept : function(std::move(callable)) { }
	template <class FPtr> constexpr Function(const FPtr& callable) noexcept : function(callable) { }
	template <class FPtr> constexpr Function(FPtr&& callable) noexcept : function(std::move(callable)) { }

	constexpr Function& operator=(const wrapper& function) noexcept {
		this->function = function.function;
	}
	constexpr Function& operator=(wrapper&& function) noexcept {
		this->function = std::move(function.function);
	}
	constexpr Function& operator=(const callable& callable) noexcept {
		this->function = callable;
	}
	constexpr Function& operator=(callable&& callable) noexcept {
		this->function = std::move(callable);
	}

	constexpr void swap(wrapper& second) noexcept {
		std::swap(this->function, second->function);
	}
	constexpr void swap(callable& second) noexcept {
		std::swap(this->function, second);
	}
	constexpr void swap(wrapper&& second) noexcept {
		std::swap(this->function, std::move(second->function));
	}
	constexpr void swap(callable&& second) noexcept {
		std::swap(this->function, std::move(second));
	}

	constexpr const std::type_info& target_type() const noexcept {
		if (!std::is_same_v<Ty, void>) return typeid(Ty);
		return typeid(void);	
	}
	template<class TTy> constexpr TTy* target() noexcept {
		if (target_type() == typeid(TTy)) return function;
		return nullptr;
	}
	template<class TTy> constexpr const TTy* target() const noexcept{
		if (target_type() == typeid(TTy)) return function;
		return nullptr;
	}

	constexpr operator bool() const noexcept {
		return static_cast<bool>(function);
	}
	constexpr result operator()(Args&&... arguments) const {
		return (*function)(std::forward<Args>(arguments)...);
	}

private:
	callable function = nullptr;
};

} // namespace lyra

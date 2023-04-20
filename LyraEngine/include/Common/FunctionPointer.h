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

/**
 * @brief a basic function pointer wrapper
 * 
 * @tparam Ty function pointer type
 * @tparam Args function arguments 
 */
template <class Ty, class... Args> class Function <Ty(Args...)> {
public:
	using result = Ty;
	using wrapper = Function;
	typedef result(*callable)(Args...); // don't know how to make this with using, but this is good enough anyways

	constexpr Function() noexcept = default;
	/**
	 * @brief constuct a new function using another function pointer wrapper
	 * 
	 * @param function the other function
	 */
	constexpr Function(const wrapper& function) noexcept : function(function.function) { }
	/**
	 * @brief constuct a new function using another function pointer wrapper
	 * 
	 * @param function the other function
	 */
	constexpr Function(wrapper&& function) noexcept : function(std::move(function.function)) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @param callable the other callable
	 */
	constexpr Function(const callable& callable) noexcept : function(callable) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @param callable the other callable
	 */
	constexpr Function(callable&& callable) noexcept : function(std::move(callable)) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @tparam FPtr callable type
	 * 
	 * @param callable the other callable
	 */
	template <class FPtr> constexpr Function(const FPtr& callable) noexcept : function(callable) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @tparam FPtr callable type
	 * 
	 * @param callable the other callable
	 */
	template <class FPtr> constexpr Function(FPtr&& callable) noexcept : function(std::move(callable)) { }

	/**
	 * @brief copy the internal callable of another function into the current one
	 * 
	 * @param function the function
	 * @return Function& 
	 */
	constexpr Function& operator=(const wrapper& function) noexcept {
		this->function = function.function;
	}
	/**
	 * @brief copy the internal callable of another function into the current one
	 * 
	 * @param function the function
	 * @return Function& 
	 */
	constexpr Function& operator=(wrapper&& function) noexcept {
		this->function = std::move(function.function);
	}
	/**
	 * @brief copy a raw callable into the internal callable of the current function pointer wrapper
	 * 
	 * @param callable the callable
	 * @return Function& 
	 */
	constexpr Function& operator=(const callable& callable) noexcept {
		this->function = callable;
	}
	/**
	 * @brief copy a raw callable into the internal callable of the current function pointer wrapper
	 * 
	 * @param callable the callable
	 * @return Function& 
	 */
	constexpr Function& operator=(callable&& callable) noexcept {
		this->function = std::move(callable);
	}

	/**
	 * @brief swap the internal callable with the callable from another function
	 * 
	 * @param second second function pointer wrapper
	 */
	constexpr void swap(wrapper& second) noexcept {
		std::swap(this->function, second->function);
	}
	/**
	 * @brief swap the internal callable with another callable
	 * 
	 * @param second second function pointer
	 */
	constexpr void swap(callable& second) noexcept {
		std::swap(this->function, second);
	}
	/**
	 * @brief swap the internal callable with the callable from another function
	 * 
	 * @param second second function pointer wrapper
	 */
	constexpr void swap(wrapper&& second) noexcept {
		std::swap(this->function, std::move(second->function));
	}
	/**
	 * @brief swap the internal callable with another callable
	 * 
	 * @param second second function pointer
	 */
	constexpr void swap(callable&& second) noexcept {
		std::swap(this->function, std::move(second));
	}

	/**
	 * @brief check if function currently contains a callable object
	 * 
	 * @return true when callable is valid
	 * @return false when callable is not valid
	 */
	constexpr operator bool() const noexcept {
		return static_cast<bool>(function);
	}
	/**
	 * @brief call the internal function
	 * 
	 * @param args function arguments if necceceary
	 * @return lyra::Function::return
	 */
	constexpr result operator()(Args&&... arguments) const {
		return (*function)(std::forward<Args>(arguments)...);
	}

private:
	callable function = nullptr;
};

} // namespace lyra

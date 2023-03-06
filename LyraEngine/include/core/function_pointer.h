/*************************
 * @file function_pointer.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a basic wrapper around a C style function pointer
 * 
 * @date 2022-12-05
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

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
	typedef Ty result_type;
	typedef Function wrapper_type;
	typedef result_type(*callable_type)(Args...);

	constexpr Function() noexcept = default;
	/**
	 * @brief constuct a new function using another function pointer wrapper
	 * 
	 * @param function the other function
	 */
	constexpr Function(const wrapper_type& function) noexcept : function(function.function) { }
	/**
	 * @brief constuct a new function using another function pointer wrapper
	 * 
	 * @param function the other function
	 */
	constexpr Function(wrapper_type&& function) noexcept : function(std::move(function.function)) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @param callable the other callable
	 */
	constexpr Function(const callable_type& callable) noexcept : function(callable) { }
	/**
	 * @brief constuct a new function using another callable
	 * 
	 * @param callable the other callable
	 */
	constexpr Function(callable_type&& callable) noexcept : function(std::move(callable)) { }
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
	constexpr Function& operator=(const wrapper_type& function) noexcept {
		this->function = function.function;
	}
	/**
	 * @brief copy the internal callable of another function into the current one
	 * 
	 * @param function the function
	 * @return Function& 
	 */
	constexpr Function& operator=(wrapper_type&& function) noexcept {
		this->function = std::move(function.function);
	}
	/**
	 * @brief copy a raw callable into the internal callable of the current function pointer wrapper
	 * 
	 * @param callable the callable
	 * @return Function& 
	 */
	constexpr Function& operator=(const callable_type& callable) noexcept {
		this->function = callable;
	}
	/**
	 * @brief copy a raw callable into the internal callable of the current function pointer wrapper
	 * 
	 * @param callable the callable
	 * @return Function& 
	 */
	constexpr Function& operator=(callable_type&& callable) noexcept {
		this->function = std::move(callable);
	}

	/**
	 * @brief swap the internal callable with the callable from another function
	 * 
	 * @param second second function pointer wrapper
	 */
	constexpr void swap(wrapper_type& second) noexcept {
		std::swap(this->function, second->function);
	}
	/**
	 * @brief swap the internal callable with another callable
	 * 
	 * @param second second function pointer
	 */
	constexpr void swap(callable_type& second) noexcept {
		std::swap(this->function, second);
	}
	/**
	 * @brief swap the internal callable with the callable from another function
	 * 
	 * @param second second function pointer wrapper
	 */
	constexpr void swap(wrapper_type&& second) noexcept {
		std::swap(this->function, std::move(second->function));
	}
	/**
	 * @brief swap the internal callable with another callable
	 * 
	 * @param second second function pointer
	 */
	constexpr void swap(callable_type&& second) noexcept {
		std::swap(this->function, std::move(second));
	}

	/**
	 * @brief check if function currently contains a callable object
	 * 
	 * @return bool
	 */
	constexpr operator bool() const noexcept {
		return static_cast<bool>(function);
	}
	/**
	 * @brief call the internal function
	 * 
	 * @param args function arguments if necceceary
	 * @return lyra::Function::return_type
	 */
	constexpr result_type operator()(Args&&... arguments) const {
		return (*function)(std::forward<Args>(arguments)...);
	}

private:
	callable_type function = nullptr;
};

} // namespace lyra

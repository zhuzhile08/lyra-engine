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

/**
 * @brief a basic function pointer wrapper
 * 
 * @tparam Ty function pointer type
 * @tparam Args function arguments 
 */
template <class Ty, class... Args> class Function {
public:
    // internal function pointer type
    typedef Ty(*callable_type)(Args...);

    /**
     * @brief constuct a new function using another function pointer wrapper
     * 
     * @param function the other function
     */
    constexpr Function(const Function& function) noexcept : function(function.function) { }
    /**
     * @brief constuct a new function using another callable
     * 
     * @param callable the other callable
     */
    constexpr Function(const callable_type&& callable) noexcept : function(callable) { }
    /**
     * @brief constuct a new function using another callable
     * 
     * @tparam FPtr callable type
     * 
     * @param callable the other callable
     */
    template <class FPtr> constexpr Function(const FPtr&& callable) noexcept : function(callable) { }

    /**
     * @brief copy the internal callable of another function into the current one
     * 
     * @param function the function
     * @return Function& 
     */
    constexpr Function& operator=(const Function& function) noexcept {
        this->function = function.function;
    }
    /**
     * @brief copy a raw callable into the internal callable of the current function pointer wrapper
     * 
     * @param callable the callable
     * @return Function& 
     */
    constexpr Function& operator=(const callable_type&& callable) noexcept {
        this->function = callable;
    }

    /**
     * @brief swap the internal callable with the callable from another function
     * 
     * @param second second function pointer wrapper
     */
    void swap(Function& second) noexcept {
        std::swap(this->function, second->function);
    }
    /**
     * @brief swap the internal callable with another callable
     * 
     * @param second second function pointer
     */
    void swap(callable_type& second) noexcept {
        std::swap(this->function, second);
    }

    /**
     * @brief check if function currently contains a callable object
     * 
     * @return bool
     */
    constexpr operator bool() const noexcept {
        if (function == nullptr) return false;
        else return true;
    }

    /**
     * @brief call the internal function
     * 
     * @param args function arguments if necceceary
     * @return Ty 
     */
    constexpr Ty operator()(Args&&... arguments) const {
        return (*function)(std::forward<Args>(arguments)...);
    }

private:
    callable_type function = nullptr;;
};

} // namespace lyra

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

#include <tuple>

namespace lyra {

/**
 * @brief a basic function pointer wrapper
 * 
 * @tparam _Ty function pointer type
 * @tparam _Args function arguments 
 */
template<class _Ty, class... _Args> class Function {
public:
    // internal function pointer type
    typedef _Ty(*callable)(_Args...);
    typedef std::tuple<_Args...> arguments;

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
    constexpr Function(const callable&& callable) noexcept : function(callable) { }
    /**
     * @brief constuct a new function using another function pointer wrapper
     * 
     * @param function the other function
     * @param args function arguments
     */
    constexpr Function(const Function& function, _Args&&... args)
         : function(function.function), args(std::make_tuple(std::forward<_Args>(args)...)) { }
    /**
     * @brief constuct a new function using another callable
     * 
     * @param callable the other callable
     * @param args function arguments
     */
    constexpr Function(const callable&& callable, _Args&&... args)
         : function(callable), args(std::make_tuple(std::forward<_Args>(args)...)) { }

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
    constexpr Function& operator=(const callable&& callable) noexcept {
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
    void swap(callable& second) noexcept {
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
     * @return _Ty 
     */
    constexpr _Ty operator()(_Args... args) const {
        return (*function)(args...);
    }
    /**
     * @brief call the internal function
     * 
     * @return _Ty 
     */
    constexpr _Ty operator()() const {
        return std::apply(function, args);
    }

private:
    callable function = nullptr;
    arguments args;
};

} // namespace lyra

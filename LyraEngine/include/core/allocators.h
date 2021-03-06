/*************************
 * @file   allocators.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  classes for better heap memory allocation
 * @brief  the credit of all this code and the code of the other allocators goes to the geniuses that provided it here -> https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/
 * @brief  I have absolutely no idea how this all works, all I know is that it just works
 * 
 * @date   2022-9-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <core/core.h>

#include <math/math.h>
#include <core/logger.h>

namespace lyra {

// base version of a allocator
class BaseAllocator {
public:
	BaseAllocator() { }

	/**
	 * @brief create a allocator
	 * 
	 * @param size size of the allocator
	 * @param start starting address of the allocator
	 */
	BaseAllocator(uint32 size, void* start);

	/**
	 * @brief destruct the allocator
	 */
	virtual ~BaseAllocator();

	[[nodiscard]] virtual void* alloc(size_t size, uint8 alignment = 4) = 0;
	virtual void dealloc(void* p) = 0;

	/**
	 * @brief get the starting position of the allocation
	 *
	 * @return const void*
	 */
	[[nodiscard]] const void* start() const noexcept { return _start; }
	/**
	 * @brief get the size of the allocation
	 * 
	 * @return const uint32_t
	 */
	[[nodiscard]] const uint32 size() const noexcept { return _size; }
	/**
	 * @brief get total used memory
	 *
	 * @return const uint32_t
	 */
	[[nodiscard]] const uint32 usedMemory() const noexcept { return _usedMemory; }
	/**
	 * @brief get the number of allocations
	 *
	 * @return const uint32_t
	 */
	[[nodiscard]] const uint32 allocCount() const noexcept { return _allocCount; }

protected:
	void* _start;
	uint32 _size;
	uint32 _usedMemory = 0;
	uint32 _allocCount = 0;
};

// linear allocator
class LinearAllocator : public BaseAllocator {
public:
	/**
	 * @brief destruct the linear allocator
	 */
	~LinearAllocator();

	/**
	 * @brief allocate memory
	 * 
	 * @param size size of the allocation
	 * @param aligment allocation alignment
	 * 
	 * @return void*
	 */
	[[nodiscard]] void* alloc(size_t size, uint8 alignment = 4) override;
#ifdef _DEBUG
	/**
	 * @brief WARNING! This function is not available in a linear allocator and will not be available in the release build
	 */
	void dealloc(void* p) override;
#else
	void dealloc(void* p) override = delete;
#endif
	/**
	 * @brief clear the linear allocator
	 */
	void clear();

private:
	LinearAllocator operator=(const LinearAllocator&);
	void* _currentPos;
};

// linear allocator
class StackAllocator : public BaseAllocator {

};

// linear allocator
class PoolAllocator : public BaseAllocator {

};

// a wrapper around the allocation functions and pointer mathematics
struct Allocator {
public:
	/**
	 * @brief allocate unitinialized memory
	 *
	 * @tparam _Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 */
	template <class _Ty> [[nodiscard]] static _Ty* allocate(BaseAllocator* allocator) {
		return new(allocator->alloc(sizeof(_Ty), __alignof(_Ty))) _Ty;
	}

	/**
	 * @brief allocate inialized memory
	 *
	 * @tparam _Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param value to initialize the memory
	 */
	template <class _Ty> [[nodiscard]] static _Ty* allocate(BaseAllocator* allocator, _Ty value) {
		return new (allocator->alloc(sizeof(_Ty), __alignof(_Ty))) _Ty(value);
	}

	/**
	 * @brief deallocate memory
	 *
	 * @tparam _Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param object object to deallocate
	 */
	template <class _Ty> static void deallocate(BaseAllocator* allocator, _Ty object) {
		object.~_Ty();
		allocator->dealloc(object);
	}

	/**
	 * @brief allocate array
	 *
	 * @tparam _Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param length length of the array
	 */
	template <class _Ty> [[nodiscard]] static _Ty* allocate_array(BaseAllocator* allocator, size_t length) {
		lassert(length == 0, "Attemted to allocate an array with a length of 0!");
		uint32 headerSize{ sizeof(size_t) / sizeof(_Ty) };

		if (sizeof(size_t) % sizeof(T) > 0) headerSize += 1;

		_Ty* p = ((_Ty)allocator->alloc(sizeof(_Ty) * (length + headerSize), __alignof(_Ty))) + headerSize;
		*(((size_t*)p) - 1) = length;

		for (int i = 0; i < length; i++) new (&p) _Ty;

		return p
	}

	/**
	 * @brief deallocate array
	 *
	 * @tparam _Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param array array to deallocate
	 */
	template <class _Ty> static void deallocate_array(BaseAllocator* allocator, _Ty* array) {
		lassert(array == nullptr, "Attemted to deallocate an array which is a nullpointer!");
		size_t length = *(((size_t*)array) - 1);

		for (size_t i = 0; i < length; i++) array.~T();

		uint32 headerSize = sizeof(size_t) / sizeof(T);
		if (sizeof(size_t) % sizeof(T) > 0) headerSize += 1;

		allocator.dealloc(array - headerSize);
	}

private:
	Allocator() noexcept = delete;
	Allocator(Allocator&) noexcept = delete;
	Allocator operator=(const Allocator&) const noexcept = delete;
};

} // namespace lyra

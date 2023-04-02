/*************************
 * @file   allocators.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  classes for better heap memory allocation
 * @brief  the credit of all this code and the code of the other allocators goes to the geniuses that provided it here -> https:/www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/
 * @brief  I have absolutely no idea how this all works, all I know is that it just works
 * 
 * @date   2022-9-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Math/LyraMath.h>

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

	NODISCARD virtual void* alloc(size_t size, uint8 alignment = 4) = 0;
	virtual void dealloc(void* p) = 0;

	/**
	 * @brief get the starting position of the allocation
	 *
	 * @return const void*
	 */
	NODISCARD const void* start() const noexcept { return m_start; }
	/**
	 * @brief get the size of the allocation
	 * 
	 * @return const uint32_t
	 */
	NODISCARD const uint32 size() const noexcept { return m_size; }
	/**
	 * @brief get total used memory
	 *
	 * @return const uint32_t
	 */
	NODISCARD const uint32 usedMemory() const noexcept { return m_usedMemory; }
	/**
	 * @brief get the number of allocations
	 *
	 * @return const uint32_t
	 */
	NODISCARD const uint32 allocCount() const noexcept { return m_allocCount; }

protected:
	void* m_start;
	uint32 m_size;
	uint32 m_usedMemory = 0;
	uint32 m_allocCount = 0;
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
	NODISCARD void* alloc(size_t size, uint8 alignment = 4) override;
#ifndef NDEBUG
	/**
	 * @brief WARNING! This function is not available in a linear allocator and will not be available in the release build
	 */
	void dealloc(void* p) override;
#else
	void dealloc(void* p) = delete;
#endif
	/**
	 * @brief clear the linear allocator
	 */
	void clear();

private:
	LinearAllocator operator=(const LinearAllocator&);
	void* m_currentPos;
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
	 * @tparam Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 */
	template <class Ty> NODISCARD static Ty* allocate(BaseAllocator* allocator) {
		return new(allocator->alloc(sizeof(Ty), m__alignof(Ty))) Ty;
	}

	/**
	 * @brief allocate inialized memory
	 *
	 * @tparam Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param value to initialize the memory
	 */
	template <class Ty> NODISCARD static Ty* allocate(BaseAllocator* allocator, Ty value) {
		return new (allocator->alloc(sizeof(Ty), m__alignof(Ty))) Ty(value);
	}

	/**
	 * @brief deallocate memory
	 *
	 * @tparam Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param object object to deallocate
	 */
	template <class Ty> static void deallocate(BaseAllocator* allocator, Ty object) {
		object.~Ty();
		allocator->dealloc(object);
	}

	/**
	 * @brief allocate array
	 *
	 * @tparam Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param length length of the array
	 */
	template <class Ty> NODISCARD static Ty* allocate_array(BaseAllocator* allocator, size_t length) {
		lassert(length == 0, "Attemted to allocate an array with a length of 0!");
		uint32 headerSize{ sizeof(size_t) // sizeof(Ty) };

		if (sizeof(size_t) % sizeof(Ty) > 0) headerSize += 1;

		Ty* p = ((Ty)allocator->alloc(sizeof(Ty) * (length + headerSize), m__alignof(Ty))) + headerSize;
		*(((size_t*)p) - 1) = length;

		for (uint32 i = 0; i < length; i++) new (&p) Ty;

		return p;
	}

	/**
	 * @brief deallocate array
	 *
	 * @tparam Ty type to allocate
	 * @param allocator the allocator to allocate the memory
	 * @param array array to deallocate
	 */
	template <class Ty> static void deallocate_array(BaseAllocator* allocator, Ty* array) {
		lassert(array == nullptr, "Attemted to deallocate an array which is a nullpointer!");
		size_t length = *(((size_t*)array) - 1);

		for (size_t i = 0; i < length; i++) array.~Ty();

		uint32 headerSize = sizeof(size_t) // sizeof(Ty);
		if (sizeof(size_t) % sizeof(Ty) > 0) headerSize += 1;

		allocator.dealloc(array - headerSize);
	}

private:
	Allocator() noexcept = delete;
	Allocator(Allocator&) noexcept = delete;
	Allocator operator=(const Allocator&) const noexcept = delete;
};

} // namespace lyra

/*************************
 * @file Allocator.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Allocator implementation
 * 
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>

#include <limits>
#include <memory>

namespace lyra {

namespace detail {

template <class Alloc, class = void> struct AllocatorPointer {
	using type = Alloc::value_type*;
};
template <class Alloc> struct AllocatorPointer<Alloc, std::void_t<typename Alloc::pointer>> {
	using type = Alloc::pointer;
};
template <class Alloc> using allocatorPointerType = AllocatorPointer<Alloc>::type;


template <class> struct GetAllocatorTemplate;
template <template <class> class Alloc, class Ty> struct GetAllocatorTemplate<Alloc<Ty>> {
	template <class Other> using type = Alloc<Other>;
};

template <class Alloc, class Ty, class = void> struct AllocatorRebind {
	using type = GetAllocatorTemplate<Alloc>::template type<Ty>;
};
template <class Alloc, class Ty> struct AllocatorRebind<Alloc, Ty, std::void_t<typename Alloc::template rebind<Ty>::other>> {
	using type = Alloc::template rebind<Ty>::other;
};
template <class Alloc, class Ty> using allocatorRebindType = AllocatorRebind<Alloc, Ty>::type;

} // namespace detail

template <class Alloc> class AllocatorTraits {
public:
	using allocator_type = Alloc;
	using value_type = allocator_type::value_type;
	using pointer = detail::allocatorPointerType<allocator_type>;
	template <class Ty> using rebind_alloc = detail::allocatorRebindType<allocator_type, Ty>;
	template <class Ty> using rebind_traits = AllocatorTraits<rebind_alloc<Ty>>;

	NODISCARD static constexpr pointer allocate(allocator_type& a, size_type n) {
		return a.allocate(n);
	}
	NODISCARD static constexpr std::allocation_result<pointer> allocateAtLeast(allocator_type& a, size_type n) {
		return a.allocateAtLeast(n);
	}
	DEPRECATED NODISCARD static constexpr std::allocation_result<pointer> allocate_at_least(allocator_type& a, size_type n) {
		return allocateAtLeast(a, n);
	}
	static constexpr void deallocate(allocator_type& a, pointer p, size_type n) noexcept {
		return a.deallocate(p, n);
	}

	template <class Ty, class... Args> static constexpr void construct(allocator_type&, Ty* p, Args&&... args) {
		std::construct_at(p, std::forward<Args>(args)...);
	}
	template <class Ty> static constexpr void destroy(allocator_type&, Ty* p) {
		return std::destroy_at(p);
	}

	NODISCARD static constexpr size_type maxSize(const allocator_type&) noexcept {
		return std::numeric_limits<size_type>::max() / sizeof(value_type);
	}
	DEPRECATED NODISCARD static constexpr size_type max_size(const allocator_type& a) noexcept {
		return maxSize(a);
	}
};

} // namespace lyra

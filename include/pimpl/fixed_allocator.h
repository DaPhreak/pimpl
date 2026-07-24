#pragma once

#include <cassert>
#include <cstddef>
#include <new>
#include <type_traits>

namespace phreak {

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
class fixed_allocator {
public:

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = T const*;

    using reference       = T&;
    using const_reference = T const&;

    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;
    
    using allocation_free = std::true_type;

    constexpr fixed_allocator () noexcept {}
    constexpr fixed_allocator (fixed_allocator const& S) noexcept {}

    constexpr fixed_allocator& operator = (fixed_allocator const& S) noexcept
    {
        return *this;
    }

    [[nodiscard]] constexpr T* allocate(const size_t Count) noexcept
    {
        return const_cast<T*>(const_cast<fixed_allocator const&>(*this).allocate(Count));
    }

    [[nodiscard]] constexpr T const* allocate(const size_t Count) const noexcept
    {
        static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
        static_assert(sizeof(value_type) <= sizeof(mStorage),"size of value_type is too big!");
        static_assert(alignof(value_type) <= Align,"alignment of value_type is too big!");
        assert(sizeof(value_type) * Count <= sizeof(mStorage));

        return std::launder(reinterpret_cast<T const*>(mStorage));
    }

    constexpr void deallocate(T const* P,const size_t Count) const noexcept{}

private:

    alignas(Align) std::byte mStorage[MaxSpace];

};

} //namespace phreak

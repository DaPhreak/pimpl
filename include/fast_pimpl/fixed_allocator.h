#pragma once

#include <type_traits>
#include <stdexcept>

namespace phreak {

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
class fixed_allocator {
public:

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using allocation_free = std::true_type;

    constexpr fixed_allocator () noexcept {}
    constexpr fixed_allocator (fixed_allocator const& S) noexcept {}
    ~fixed_allocator () noexcept
    {
        static_assert(sizeof(T)<=sizeof(mStorage),"Size of T is too big!");
        static_assert(alignof(T)<=Align,"Alignment of T is too big!");
    }

    constexpr fixed_allocator& operator = (fixed_allocator const& S) noexcept
    {
        return *this;
    }

    [[nodiscard]] constexpr T* allocate(const size_t Count)
    {
        static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
        if ( sizeof(value_type) * Count > sizeof(mStorage) ) {
            throw std::runtime_error{ "allocation failed!" };
        }
        return std::launder(reinterpret_cast<T*>(mStorage));
    }

    [[nodiscard]] constexpr T const* allocate(const size_t Count) const
    {
        static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
        if ( sizeof(value_type) * Count > sizeof(mStorage) ) {
            throw std::runtime_error{ "allocation failed!" };
        }
        return std::launder(reinterpret_cast<T const*>(mStorage));
    }

    constexpr void deallocate(T const* P,const size_t Count) const noexcept{}

private:

    alignas(Align) std::byte mStorage[MaxSpace];

};

} //namespace phreak

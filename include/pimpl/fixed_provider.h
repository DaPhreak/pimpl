#pragma once

#include "provider.h"

#include <new>
#include <cstddef>

namespace phreak::detail {

template <class T,size_t MaxSpace,size_t Align,class Eraser=void>
class fixed_provider: public provider<fixed_provider<T,MaxSpace,Align,Eraser>> {
public:

    using eraser_type   = Eraser;
    using value_type    = T;
    using pointer       = T*;
    using const_pointer = T const*;

public:

    constexpr fixed_provider() noexcept
    {
        static_assert(sizeof(value_type) <= MaxSpace,"size of value_type is too big!");
        static_assert(alignof(value_type) <= Align,"alignment of value_type is too big!");
    }
    ~fixed_provider()
    {
        this->destroy();
    }
public:

    constexpr const_pointer data() const noexcept
    {
       return std::launder(reinterpret_cast<const_pointer>(mStorage));
    };
    constexpr pointer data() noexcept
    {
        return std::launder(reinterpret_cast<pointer>(mStorage));
    };

private:

    alignas(Align) std::byte mStorage[MaxSpace];

};

} //namespace phreak::detail

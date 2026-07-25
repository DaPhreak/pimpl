#pragma once

#include "pimpl.h"

#include <cstddef>

namespace phreak {

template <size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
struct aligned_storage {
    alignas(Align) std::byte storage[MaxSpace];
};

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
using fast_pimpl = pimpl<T,aligned_storage<MaxSpace,Align>>;

} //namespace phreak

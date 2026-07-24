#pragma once

#include "fixed_allocator.h"
#include "pimpl.h"

namespace phreak {

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
using fast_pimpl = pimpl<T,fixed_allocator<T,MaxSpace,Align>>;

} //namespace phreak

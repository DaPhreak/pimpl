#pragma once

#include "pimpl_holder.h"
#include "fixed_provider.h"

namespace phreak {

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
using fast_pimpl = pimpl_holder<fixed_provider<T,MaxSpace,Align>>;

} //namespace phreak

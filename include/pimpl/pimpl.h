#pragma once

#include "pimpl_holder.h"
#include "dynamic_provider.h"

namespace phreak {

template <class T,class Alloc = std::allocator<T>>
using pimpl = pimpl_holder<T,dynamic_provider<Alloc>>;

} //namespace phreak

#pragma once

#include "pimpl_holder.h"
#include "dynamic_provider.h"

#include <memory>

namespace phreak {

template <class T,class Alloc = std::allocator<T>>
using pimpl = pimpl_holder<detail::dynamic_provider<Alloc>>;

} //namespace phreak

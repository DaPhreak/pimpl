#pragma once

#include <memory>

namespace phreak {

template <class T,class Alloc = std::allocator<T>>
class dynamic_provider: private Alloc {
public:

    using value_type      = T;
    using allocator_type  = Alloc;
    using pointer         = T*;
    using const_pointer	  = T const*;

public:

    dynamic_provider() 
    : mData{this->allocate(1)}
    {}
    dynamic_provider(dynamic_provider const& S)
    : Alloc{S}
    , mData{this->allocate(1)}
    {}
    dynamic_provider(dynamic_provider&& S)
    : Alloc{std::move(S)}
    , mData{this->allocate(1)}
    {}
    ~dynamic_provider()
    {
        this->deallocate(mData,1);
    }
    dynamic_provider& operator = (dynamic_provider const&) = delete;

public:

    constexpr const_pointer data() const noexcept
    {
        return mData;
    };
    constexpr pointer data() noexcept
    {
        return mData;
    };

private:

    pointer mData{};

};

} //namespace phreak

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

    dynamic_provider() noexcept(is_nothrow_alloc)
    : mData{this->allocate(1)}
    {}
    dynamic_provider(dynamic_provider const& S) noexcept(is_nothrow_alloc)
    : Alloc{S}
    , mData{this->allocate(1)}
    {}
    dynamic_provider(dynamic_provider&& S) noexcept(is_nothrow_alloc)
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
    constexpr static bool is_nothrow_alloc{std::is_nothrow_invocable_v<decltype(std::declval<Alloc>().allocate(1))>};
    pointer mData{};

};

} //namespace phreak

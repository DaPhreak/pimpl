#pragma once

#include <type_traits>

namespace phreak::detail {

template <class Alloc>
class dynamic_provider: private Alloc {
public:

    using allocator_type = Alloc;
    using value_type     = typename Alloc::value_type;
    using pointer        = value_type*;
    using const_pointer	 = value_type const*;

public:

    dynamic_provider() noexcept(is_nothrow_alloc)
    {}
    dynamic_provider(dynamic_provider const& S) noexcept(is_nothrow_alloc && std::is_nothrow_copy_constructible_v<Alloc>)
    : Alloc{S}
    {}
    dynamic_provider(dynamic_provider&& S) noexcept(is_nothrow_alloc && std::is_nothrow_move_constructible_v<Alloc>)
    : Alloc{std::move(S)}
    {}
    template<class... Args,typename = std::enable_if_t<std::is_constructible_v<Alloc,Args...>>>
    explicit dynamic_provider(Args&&... args) noexcept(is_nothrow_alloc && std::is_nothrow_constructible_v<Alloc,Args...>)
    : Alloc{std::forward<Args>(args)...}
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

    pointer mData{this->allocate(1)};

};

} //namespace phreak::detail

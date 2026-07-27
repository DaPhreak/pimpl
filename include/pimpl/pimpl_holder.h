#pragma once

#include <new>
#include <utility>
#include <type_traits>

namespace phreak {

struct provider_arg_t { explicit provider_arg_t() = default; };
constexpr provider_arg_t provider_arg{};

template <class Provider>
class pimpl_holder {
public:

    using provider_type   = Provider;
    using value_type      = typename Provider::value_type;
    using pointer         = value_type*;
    using const_pointer	  = value_type const*;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using rv_ref          = value_type&&;
    using rv_const_ref    = value_type const&&;

    template <class U = value_type,typename = std::enable_if_t<std::is_default_constructible_v<U>>>
    pimpl_holder () noexcept(std::is_nothrow_constructible_v<value_type> && std::is_nothrow_constructible_v<provider_type>)
    {
        mProvider.create();
    }

    //template <class U = value_type,typename = std::enable_if_t<std::is_copy_constructible_v<U>>> -> needs concepts
    pimpl_holder (pimpl_holder const& S) noexcept(std::is_nothrow_copy_constructible_v<value_type> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{S.mProvider}
    {
        static_assert(std::is_copy_constructible_v<value_type>,"value_type is not copy constructible");
        mProvider.create(*S);
    }

    //template <class U = value_type,typename = std::enable_if_t<std::is_move_constructible_v<U>>> -> needs concepts
    pimpl_holder (pimpl_holder&& S) noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_move_constructible_v<provider_type>)
    : mProvider{std::move(S.mProvider)}
    {
        static_assert(std::is_move_constructible_v<value_type>,"value_type is not move constructible");
        mProvider.create(std::move(*S));
    }

    template<class... Args,typename = std::enable_if_t<std::is_constructible_v<value_type,Args...>>>
    explicit pimpl_holder (Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type,Args...> && std::is_nothrow_constructible_v<provider_type>)
    {
        new(mProvider.data()) value_type(std::forward<Args>(args)...);
    }

    template<class P,class... Args,typename = std::enable_if_t<std::is_constructible_v<value_type,Args...>>>
    explicit pimpl_holder (provider_arg_t,P&& p,Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type,Args...> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{std::forward<P>(p)}
    {
        new(mProvider.data()) value_type(std::forward<Args>(args)...);
    }

    //template <class U = value_type,typename = std::enable_if_t<std::is_copy_assignable_v<U>>> -> needs concepts
    pimpl_holder& operator = (pimpl_holder const& S) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    {
        static_assert(std::is_copy_assignable_v<value_type>,"value_type is not copy assignable");
        **this=*S;
        return *this;
    }

    //template <class U = value_type,typename = std::enable_if_t<std::is_move_assignable_v<U>>> -> needs concepts
    pimpl_holder& operator = (pimpl_holder&& S) noexcept(std::is_nothrow_move_assignable_v<value_type>)
    {
        static_assert(std::is_move_assignable_v<value_type>,"value_type is not move assignable");
        **this=std::move(*S);
        return *this;
    }

    template <class U = value_type,typename = std::enable_if_t<std::is_assignable_v<value_type,U>>>
    pimpl_holder& operator = (U&& S) noexcept(std::is_nothrow_assignable_v<value_type,U>)
    {
        **this=std::forward<U>(S);
        return *this;
    }

public:

    [[nodiscard]] constexpr const_pointer   operator -> () const   noexcept { return &(**this); }
    [[nodiscard]] constexpr pointer         operator -> ()         noexcept { return &(**this); }

    [[nodiscard]] constexpr const_reference operator *  () const&  noexcept { return *mProvider.data(); }
    [[nodiscard]] constexpr reference       operator *  () &       noexcept { return *mProvider.data(); }
    [[nodiscard]] constexpr rv_const_ref    operator *  () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr rv_ref          operator *  () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr const_reference value       () const&  noexcept { return **this; }
    [[nodiscard]] constexpr reference       value       () &       noexcept { return **this; }
    [[nodiscard]] constexpr rv_const_ref    value       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr rv_ref          value       () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr operator const_reference    () const&  noexcept { return **this; }
    [[nodiscard]] constexpr operator reference          () &       noexcept { return **this; }
    [[nodiscard]] constexpr operator rv_const_ref       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr operator rv_ref             () &&      noexcept { return std::move(**this); }

    void swap (pimpl_holder& Other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        std::swap(**this,*Other);
    }

private:

    provider_type mProvider{};

};


template <class Provider>
[[nodiscard]] bool operator == (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs == *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator == (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs == rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator == (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs == *rhs;
}

template <class Provider>
[[nodiscard]] bool operator != (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs != *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator != (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs != rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator != (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs != *rhs;
}

template <class Provider>
[[nodiscard]] bool operator < (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs < *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator < (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs < rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator < (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs < *rhs;
}

template <class Provider>
[[nodiscard]] bool operator <= (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs <= *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator <= (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs <= rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator <= (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs <= *rhs;
}

template <class Provider>
[[nodiscard]] bool operator > (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs > *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator > (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs > rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator > (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs > *rhs;
}

template <class Provider>
[[nodiscard]] bool operator >= (pimpl_holder<Provider> const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return *lhs >= *rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator >= (pimpl_holder<Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs >= rhs;
}

template <class Provider,class U>
[[nodiscard]] bool operator >= (U const& lhs,pimpl_holder<Provider> const& rhs) noexcept
{
    return lhs >= *rhs;
}

} //namespace phreak

namespace std {

template <class Provider>
void swap (phreak::pimpl_holder<Provider>& lhs,phreak::pimpl_holder<Provider>& rhs) noexcept(std::is_nothrow_swappable_v<typename Provider::value_type>)
{
    lhs.swap(rhs);
}

} // namespace std

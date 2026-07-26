#pragma once

#include <new>
#include <utility>
#include <type_traits>

namespace phreak {

struct provider_arg_t { explicit provider_arg_t() = default; };
constexpr provider_arg_t provider_arg{};

template <class T,class Provider>
class pimpl_holder {
public:

    using value_type      = T;
    using provider_type   = Provider;
    using pointer         = T*;
    using const_pointer	  = T const*;
    using reference       = T&;
    using const_reference = T const&;

    template <class U = T,typename = std::enable_if_t<std::is_default_constructible_v<U>>>
    pimpl_holder () noexcept(std::is_nothrow_constructible_v<T> && std::is_nothrow_constructible_v<provider_type>)
    {
        new(mProvider.data()) T{};
    }

    //template <class U = T,typename = std::enable_if_t<std::is_copy_constructible_v<U>>> -> needs concepts
    pimpl_holder (pimpl_holder const& S) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{S.mProvider}
    {
        static_assert(std::is_copy_constructible_v<T>,"value_type is not copy constructible");
        new(mProvider.data()) T{*S};
    }

    //template <class U = T,typename = std::enable_if_t<std::is_move_constructible_v<U>>> -> needs concepts
    pimpl_holder (pimpl_holder&& S) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<provider_type>)
    : mProvider{std::move(S.mProvider)}
    {
        static_assert(std::is_move_constructible_v<T>,"value_type is not move constructible");
        new(mProvider.data()) T{std::move(*S)};
    }

    template<class... Args,typename = std::enable_if_t<std::is_constructible_v<T,Args...>>>
    explicit pimpl_holder (Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_constructible_v<provider_type>)
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }

    template<class... Args,typename = std::enable_if_t<std::is_constructible_v<T,Args...>>>
    explicit pimpl_holder (provider_type const& S,Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{S}
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }

    template<class P,class... Args,typename = std::enable_if_t<std::is_constructible_v<T,Args...>>>
    explicit pimpl_holder (provider_arg_t,P&& p,Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{std::forward<P>(p)}
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }

    ~pimpl_holder () noexcept
    {
        (**this).~T();
    }

    //template <class U = T,typename = std::enable_if_t<std::is_copy_assignable_v<U>>> -> needs concepts
    pimpl_holder& operator = (pimpl_holder const& S) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        static_assert(std::is_copy_assignable_v<T>,"value_type is not copy assignable");
        **this=*S;
        return *this;
    }

    //template <class U = T,typename = std::enable_if_t<std::is_move_assignable_v<U>>> -> needs concepts
    pimpl_holder& operator = (pimpl_holder&& S) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        static_assert(std::is_move_assignable_v<T>,"value_type is not move assignable");
        **this=std::move(*S);
        return *this;
    }

    template <class U = T,typename = std::enable_if_t<std::is_assignable_v<T,U>>>
    pimpl_holder& operator = (U&& S) noexcept(std::is_nothrow_assignable_v<T,U>)
    {
        **this=std::forward<U>(S);
        return *this;
    }

public:

    [[nodiscard]] constexpr T const&  operator *  () const&  noexcept { return *std::launder(mProvider.data()); }
    [[nodiscard]] constexpr T&        operator *  () &       noexcept { return *std::launder(mProvider.data()); }
    [[nodiscard]] constexpr T const&& operator *  () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr T&&       operator *  () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr T const*  operator -> () const   noexcept { return &(**this); }
    [[nodiscard]] constexpr T*        operator -> ()         noexcept { return &(**this); }

    [[nodiscard]] constexpr operator  T const&    () const&  noexcept { return **this; }
    [[nodiscard]] constexpr operator  T&          () &       noexcept { return **this; }
    [[nodiscard]] constexpr operator  T const&&   () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr operator  T&&         () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr T const&  value       () const&  noexcept { return **this; }
    [[nodiscard]] constexpr T&        value       () &       noexcept { return **this; }
    [[nodiscard]] constexpr T const&& value       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr T&&       value       () &&      noexcept { return std::move(**this); }

    void swap (pimpl_holder& Other) noexcept(std::is_nothrow_swappable_v<T>)
    {
        std::swap(**this,*Other);
    }

private:

    provider_type mProvider{};

};


template <class T,class Provider>
[[nodiscard]] bool operator == (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs == *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator == (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs == rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator == (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs == *rhs;
}

template <class T,class Provider>
[[nodiscard]] bool operator != (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs != *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator != (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs != rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator != (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs != *rhs;
}

template <class T,class Provider>
[[nodiscard]] bool operator < (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs < *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator < (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs < rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator < (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs < *rhs;
}

template <class T,class Provider>
[[nodiscard]] bool operator <= (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs <= *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator <= (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs <= rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator <= (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs <= *rhs;
}

template <class T,class Provider>
[[nodiscard]] bool operator > (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs > *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator > (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs > rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator > (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs > *rhs;
}

template <class T,class Provider>
[[nodiscard]] bool operator >= (pimpl_holder<T,Provider> const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return *lhs >= *rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator >= (pimpl_holder<T,Provider> const& lhs,U const& rhs) noexcept
{
    return *lhs >= rhs;
}

template <class T,class Provider,class U>
[[nodiscard]] bool operator >= (U const& lhs,pimpl_holder<T,Provider> const& rhs) noexcept
{
    return lhs >= *rhs;
}

} //namespace phreak

namespace std {

template <class T,class Provider>
void swap (phreak::pimpl_holder<T,Provider>& lhs,phreak::pimpl_holder<T,Provider>& rhs) noexcept(std::is_nothrow_swappable_v<T>)
{
    lhs.swap(rhs);
}

template <class T,class Provider>
struct hash<phreak::pimpl_holder<T,Provider>> {
    std::size_t operator()(phreak::pimpl_holder<T,Provider> const& S) const noexcept
    {
        return std::hash<T>{}(*S);
    }
};


} // namespace std

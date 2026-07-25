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

    pimpl_holder () noexcept(std::is_nothrow_constructible_v<T> && std::is_nothrow_constructible_v<provider_type>)
    {
        new(mProvider.data()) T{};
    }

    pimpl_holder (pimpl_holder const& S) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{S.mProvider}
    {
        new(mProvider.data()) T{*S};
    }

    pimpl_holder (pimpl_holder&& S) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<provider_type>)
    : mProvider{std::move(S.mProvider)}
    {
        new(mProvider.data()) T{std::move(*S)};
    }

    template<class... Args,std::enable_if_t<std::is_constructible_v<T,Args...>>* = nullptr>
    explicit pimpl_holder (Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_constructible_v<provider_type>)
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }

    template<class... Args,std::enable_if_t<std::is_constructible_v<T,Args...>>* = nullptr>
    explicit pimpl_holder (provider_type const& S,Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{S}
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }

    template<class P,class... Args,std::enable_if_t<std::is_constructible_v<T,Args...>>* = nullptr>
    explicit pimpl_holder (provider_arg_t,P&& p,Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args...> && std::is_nothrow_copy_constructible_v<provider_type>)
    : mProvider{std::forward<P>(p)}
    {
        new(mProvider.data()) T(std::forward<Args>(args)...);
    }


    ~pimpl_holder () noexcept
    {
        (**this).~T();
    }

    pimpl_holder& operator = (pimpl_holder const& S) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        **this=*S;
        return *this;
    }

    pimpl_holder& operator = (pimpl_holder&& S) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        **this=std::move(*S);
        return *this;
    }

    template <class U=T,std::enable_if_t<std::is_assignable_v<T,U>>* = nullptr>
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

} //namespace phreak

namespace std {

template <class T,class Provider>
void swap (phreak::pimpl_holder<T,Provider>& lhs,phreak::pimpl_holder<T,Provider>& rhs) noexcept(std::is_nothrow_swappable_v<T>)
{
    lhs.swap(rhs);
}

} // namespace std

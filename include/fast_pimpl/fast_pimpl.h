#pragma once

#include <new>
#include <tuple>
#include <type_traits>

namespace phreak {

template <class T,size_t MaxSpace,size_t Align=alignof(std::max_align_t)>
class fast_pimpl {
public:

    using value_type = T;

    fast_pimpl ()
    {
        new (mStorage) T{};
    }
    fast_pimpl (fast_pimpl const& S)
    {
        new (mStorage) T{*S};
    }
    fast_pimpl (fast_pimpl&& S) noexcept
    {
        new (mStorage) T{std::move(*S)};
    }
    template <class U,class... Args,std::enable_if_t<!std::is_same_v<fast_pimpl,std::decay_t<U>>>* = nullptr>
    explicit fast_pimpl (U&& arg0,Args&&... args)
    {
        new (mStorage) T(std::forward<U>(arg0),std::forward<Args>(args)...);
    }
    ~fast_pimpl () noexcept
    {
        static_assert(sizeof(T)<=sizeof(mStorage),"Size of T is too big!");
        static_assert(alignof(T)<=Align,"Alignment of T is too big!");
        (**this).~T();
    }

    fast_pimpl& operator = (fast_pimpl const& S)
    {
        **this=*S;
        return *this;
    }
    fast_pimpl& operator = (fast_pimpl&& S) noexcept
    {
        **this=std::move(*S);
        return *this;
    }

    template <class U=T,std::enable_if_t<std::is_assignable_v<T,U>>* = nullptr>
    fast_pimpl& operator = (U&& S) noexcept(std::is_nothrow_assignable_v<T,U>)
    {
        **this=std::forward<U>(S);
        return *this;
    }

public:

    constexpr T const&  operator *  () const&  noexcept { return *std::launder(reinterpret_cast<T const*>(mStorage)); }
    constexpr T&        operator *  () &       noexcept { return *std::launder(reinterpret_cast<T*      >(mStorage)); }
    constexpr T const&& operator *  () const&& noexcept { return std::move(**this); }
    constexpr T&&       operator *  () &&      noexcept { return std::move(**this); }

    constexpr T const*  operator -> () const   noexcept { return &(**this); }
    constexpr T*        operator -> ()         noexcept { return &(**this); }

    constexpr T const&  value       () const&  noexcept { return **this; }
    constexpr T&        value       () &       noexcept { return **this; }
    constexpr T const&& value       () const&& noexcept { return std::move(**this); }
    constexpr T&&       value       () &&      noexcept { return std::move(**this); }

    void swap (fast_pimpl& Other) noexcept
    {
        std::swap(**this,*Other);
    }

private:

    alignas(Align) std::byte mStorage[MaxSpace];

};

} //namespace phreak

namespace std {

template <class T,size_t MaxSpace,size_t Align>
void swap (phreak::fast_pimpl<T,MaxSpace,Align>& A,phreak::fast_pimpl<T,MaxSpace,Align>& B) noexcept
{
    A.swap(B);
}

} // namespace std

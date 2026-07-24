#pragma once

#include <new>
#include <memory>
#include <type_traits>

namespace phreak {

template <class T, class dummy = void>
struct enable_fixed_allocator_t: public std::false_type{};

template <class T>
struct enable_fixed_allocator_t<T,std::void_t<typename T::allocation_free>>: public std::true_type{};

template <class T,class A = std::allocator<T>>
class pimpl {
public:

    using value_type = T;
    using allocator_type = A;
    using pointer = T*;
    using const_pointer	= T const*;

    pimpl ()
    {
        new(mHolder.data()) T{};
    }
    pimpl (pimpl const& S)
    : mHolder{S.mHolder}
    {
        new(mHolder.data()) T{*S};
    }
    pimpl (pimpl&& S) noexcept
    : mHolder{std::move(S.mHolder)}
    {
        new(mHolder.data()) T{std::move(*S)};
    }
    template <class U,class... Args,std::enable_if_t<!std::is_same_v<pimpl,std::decay_t<U>>>* = nullptr>
    explicit pimpl (U&& arg0,Args&&... args)
    {
        new(mHolder.data()) T(std::forward<U>(arg0),std::forward<Args>(args)...);
    }
    ~pimpl () noexcept
    {
        (**this).~T();
    }

    pimpl& operator = (pimpl const& S)
    {
        **this=*S;
        return *this;
    }
    pimpl& operator = (pimpl&& S) noexcept
    {
        **this=std::move(*S);
        return *this;
    }

    template <class U=T,std::enable_if_t<std::is_assignable_v<T,U>>* = nullptr>
    pimpl& operator = (U&& S) noexcept(std::is_nothrow_assignable_v<T,U>)
    {
        **this=std::forward<U>(S);
        return *this;
    }

public:

    constexpr T const&  operator *  () const&  noexcept { return *mHolder.data(); }
    constexpr T&        operator *  () &       noexcept { return *mHolder.data(); }
    constexpr T const&& operator *  () const&& noexcept { return std::move(**this); }
    constexpr T&&       operator *  () &&      noexcept { return std::move(**this); }

    constexpr T const*  operator -> () const   noexcept { return &(**this); }
    constexpr T*        operator -> ()         noexcept { return &(**this); }

    constexpr T const&  value       () const&  noexcept { return **this; }
    constexpr T&        value       () &       noexcept { return **this; }
    constexpr T const&& value       () const&& noexcept { return std::move(**this); }
    constexpr T&&       value       () &&      noexcept { return std::move(**this); }

    void swap (pimpl& Other) noexcept
    {
        std::swap(**this,*Other);
    }

private:

    class no_alloc: public A {
    public:
        no_alloc() noexcept {} 
        no_alloc(no_alloc const& S) noexcept
        : A{S}
        {}
        no_alloc(no_alloc&& S) noexcept
        : A{std::move(S)}
        {}
    public:
        const_pointer data() const
        {
            return this->allocate(1);
        };
        pointer data()
        {
            return this->allocate(1);
        };
    };

    class compressed_pair: public A {
    public:
        compressed_pair() 
        : mData{this->allocate(1)}
        {}
        compressed_pair(compressed_pair const& S)
        : A{S}
        , mData{this->allocate(1)}
        {}
        compressed_pair(compressed_pair&& S)
        : A{std::move(S)}
        , mData{this->allocate(1)}
        {}
        ~compressed_pair()
        {
            this->deallocate(mData,1);
        }
    public:
        const_pointer data() const
        {
            return mData;
        };
        pointer data()
        {
            return mData;
        };
    private:
        pointer mData{};
    };

    using holder_t = std::conditional_t<enable_fixed_allocator_t<A>::value,no_alloc,compressed_pair>;

    holder_t mHolder{};
};

} //namespace phreak

namespace std {

template <class T,class A>
void swap (phreak::pimpl<T,A>& lhs,phreak::pimpl<T,A>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace std

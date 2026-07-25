#pragma once

#include <new>
#include <memory>
#include <type_traits>

namespace phreak {

template <class T, class dummy = void>
struct is_allocator_t: public std::false_type{};

template <class T>
struct is_allocator_t<T,std::void_t<decltype(std::declval<T&>().allocate(1))>>: public std::true_type{};

template <class T,class Alloc = std::allocator<T>>
class pimpl {
public:

    using value_type      = T;
    using allocator_type  = Alloc;
    using pointer         = T*;
    using const_pointer	  = T const*;
    using reference       = T&;
    using const_reference = T const&;

    pimpl () noexcept(std::is_nothrow_constructible_v<T> && std::is_nothrow_constructible_v<holder_t>)
    {
        new(mHolder.data()) T{};
    }

    pimpl (pimpl const& S) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_constructible_v<holder_t>)
    : mHolder{S.mHolder}
    {
        new(mHolder.data()) T{*S};
    }

    pimpl (pimpl&& S) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<holder_t>)
    : mHolder{std::move(S.mHolder)}
    {
        new(mHolder.data()) T{std::move(*S)};
    }

    template <class U,class... Args,std::enable_if_t<!std::is_same_v<pimpl,std::decay_t<U>>>* = nullptr>
    explicit pimpl (U&& arg0,Args&&... args) noexcept(std::is_nothrow_constructible_v<T,U,Args...> && std::is_nothrow_constructible_v<holder_t>)
    {
        new(mHolder.data()) T(std::forward<U>(arg0),std::forward<Args>(args)...);
    }

    ~pimpl () noexcept
    {
        (**this).~T();
    }

    pimpl& operator = (pimpl const& S) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        **this=*S;
        return *this;
    }

    pimpl& operator = (pimpl&& S) noexcept(std::is_nothrow_move_assignable_v<T>)
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

    [[nodiscard]] constexpr T const&  operator *  () const&  noexcept { return *mHolder.data(); }
    [[nodiscard]] constexpr T&        operator *  () &       noexcept { return *mHolder.data(); }
    [[nodiscard]] constexpr T const&& operator *  () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr T&&       operator *  () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr T const*  operator -> () const   noexcept { return &(**this); }
    [[nodiscard]] constexpr T*        operator -> ()         noexcept { return &(**this); }

    [[nodiscard]] constexpr T const&  value       () const&  noexcept { return **this; }
    [[nodiscard]] constexpr T&        value       () &       noexcept { return **this; }
    [[nodiscard]] constexpr T const&& value       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr T&&       value       () &&      noexcept { return std::move(**this); }

    void swap (pimpl& Other) noexcept(std::is_nothrow_swappable_v<T>)
    {
        std::swap(**this,*Other);
    }

private:

    class storage {
    public:
        constexpr storage() noexcept = default;
        constexpr storage(storage const&) noexcept
        {}
        ~storage()
        {
            static_assert(sizeof(value_type) <= sizeof(Alloc),"size of value_type is too big!");
            static_assert(alignof(value_type) <= alignof(Alloc),"alignment of value_type is too big!");
        }
        storage& operator = (storage const&) = delete;
    public:
        constexpr const_pointer data() const noexcept
        {
            return std::launder(reinterpret_cast<const_pointer>(mStorage));
        };
        constexpr pointer data() noexcept
        {
            return std::launder(reinterpret_cast<pointer>(mStorage));
        };
    private:
        alignas(alignof(Alloc)) std::byte mStorage[sizeof(Alloc)];
    };

    class compressed_pair: private Alloc {
    public:
        compressed_pair() 
        : mData{this->allocate(1)}
        {}
        compressed_pair(compressed_pair const& S)
        : Alloc{S}
        , mData{this->allocate(1)}
        {}
        compressed_pair(compressed_pair&& S)
        : Alloc{std::move(S)}
        , mData{this->allocate(1)}
        {}
        ~compressed_pair()
        {
            this->deallocate(mData,1);
        }
        compressed_pair& operator = (compressed_pair const&) = delete;
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

    using holder_t = std::conditional_t<is_allocator_t<Alloc>::value,compressed_pair,storage>;

    holder_t mHolder{};
};

} //namespace phreak

namespace std {

template <class T,class Alloc>
void swap (phreak::pimpl<T,Alloc>& lhs,phreak::pimpl<T,Alloc>& rhs) noexcept(std::is_nothrow_swappable_v<T>)
{
    lhs.swap(rhs);
}

} // namespace std

#pragma once

#include <new>
#include <type_traits>

namespace phreak::detail {

template <class This>
class provider {
public:

    provider() noexcept
    {
        if constexpr(!std::is_void_v<This::eraser_type>) {
            This::eraser_type::template register_type<This::value_type>();
        }
    }
    provider(provider const& S) noexcept {}
    provider& operator = (provider const&) = delete;

public:

    void create()
    {
        if constexpr(!std::is_void_v<This::eraser_type>) {
            This::eraser_type::construct(*cast().data());
        } else {
            new(cast().data()) This::value_type{};
        }
    }

    template<class U>
    void create(U&& Source)
    {
        if constexpr(!std::is_void_v<This::eraser_type>) {
            This::eraser_type::construct(*cast().data(),std::forward<U>(Source));
        } else {
            new(cast().data()) This::value_type{std::forward<U>(Source)};
        }
    }

    template<class U>
    void assign(U&& Source)
    {
        if constexpr(!std::is_void_v<This::eraser_type>) {
            This::eraser_type::construct(*cast().data(),std::forward<U>(Source));
        } else {
            *cast().data() = std::forward<U>(Source);
        }
    }

    void destroy() noexcept
    {
        if constexpr(!std::is_void_v<This::eraser_type>) {
            This::eraser_type::destroy(*cast().data());
        } else {
            using T = This::value_type;
            (*cast().data()).~T();
        }
    }

private:
    This& cast() { return static_cast<This&>(*this); }
    This const& cast() const { return static_cast<This const&>(*this); }
};

} //namespace phreak::detail

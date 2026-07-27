#pragma once

#include "provider.h"

#include <type_traits>

namespace phreak::detail {

template <class Alloc,class Eraser=void>
class dynamic_provider: public provider<dynamic_provider<Alloc,Eraser>>, private Alloc {
public:
    
    using eraser_type    = Eraser;
    using base_type      = provider<dynamic_provider<Alloc,Eraser>>;
    using allocator_type = Alloc;
    using value_type     = typename Alloc::value_type;
    using pointer        = value_type*;
    using const_pointer	 = value_type const*;

public:

    dynamic_provider() noexcept(is_nothrow_alloc)
    {
        register_destroy();
    }
    dynamic_provider(dynamic_provider const& S) noexcept(is_nothrow_alloc && std::is_nothrow_copy_constructible_v<Alloc>)
    : base_type{S}
    , Alloc{S}
    {}
    dynamic_provider(dynamic_provider&& S) noexcept(is_nothrow_alloc && std::is_nothrow_move_constructible_v<Alloc>)
    : base_type{S}
    , Alloc{std::move(S)}
    {}
    template<class... Args,std::enable_if_t<std::is_constructible_v<Alloc,Args...>>* = nullptr>
    explicit dynamic_provider(Args&&... args) noexcept(is_nothrow_alloc && std::is_nothrow_constructible_v<Alloc,Args...>)
    : Alloc{std::forward<Args>(args)...}
    {
        register_destroy();
    }

    ~dynamic_provider()
    {
        if constexpr(!std::is_void_v<Eraser>) {
           Eraser::destroy(*this);
        } else {
            mData->~value_type();
            this->deallocate(mData,1);
        }
    }

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

    static void register_destroy() noexcept
    {
        if constexpr(!std::is_void_v<Eraser>) {
            Eraser::template register_destroy<dynamic_provider>([](dynamic_provider& provider)
            {
                auto data{provider.data()};

                data->~value_type();
                provider.deallocate(data,1);
            });
        }
    }

    pointer mData{this->allocate(1)};

};

} //namespace phreak::detail

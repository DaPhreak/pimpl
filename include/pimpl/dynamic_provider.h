#pragma once

#include <type_traits>

namespace phreak::detail {

template <class Alloc,class Destroyer=void>
class dynamic_provider: private Alloc {
public:

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
    : Alloc{S}
    {
        register_destroy();
    }
    dynamic_provider(dynamic_provider&& S) noexcept(is_nothrow_alloc && std::is_nothrow_move_constructible_v<Alloc>)
    : Alloc{std::move(S)}
    {
        register_destroy();
    }
    template<class... Args,std::enable_if_t<std::is_constructible_v<Alloc,Args...>>* = nullptr>
    dynamic_provider(Args&&... args) noexcept(is_nothrow_alloc && std::is_nothrow_constructible_v<Alloc,Args...>)
    : Alloc{std::forward<Args>(args)...}
    {
        register_destroy();
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

    void destroy()
    {
        if constexpr(!std::is_void_v<Destroyer>) {
           Destroyer::destroy(this);
        } else {
            mData->~value_type();
            this->deallocate(mData,1);
        }
    }

private:

    constexpr static bool is_nothrow_alloc{std::is_nothrow_invocable_v<decltype(std::declval<Alloc>().allocate(1))>};

    static void register_destroy() noexcept
    {
        if constexpr(!std::is_void_v<Destroyer>) {
            Destroyer::template register_destroy<dynamic_provider>([](void* Ressource)
            {
                auto provider{ static_cast<dynamic_provider*>(Ressource) };
                auto data{provider->data()};

                data->~value_type();
                provider->deallocate(data,1);
            });
        }
    }

    pointer mData{this->allocate(1)};

};

} //namespace phreak::detail

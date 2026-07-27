#pragma once

#include <cassert>
#include <new>
#include <stdexcept>
#include <memory>

namespace phreak::detail {

class type_erasure {
public:

    template<class T>
    static void construct(T& Ressource)
    {
        if (const auto& d{default_constructor<T>()}) {
            (*d)(Ressource);
        } else {
            throw std::runtime_error{"not default constructible"};
        }
    }

    template<class T,class U>
    static void construct(T& Dest,U&& Source)
    {
        if (const auto& d{assign_constructor<T>()}) {
            (*d)(Dest,std::forward<U>(Source));
        } else {
            throw std::runtime_error{"not assign constructible"};
        }
    }

    template<class T,class U>
    static void assign(T& Dest,U&& Source)
    {
        if (const auto& d{assigner<T>()}) {
            (*d)(Dest,std::forward<U>(Source));
        } else {
            throw std::runtime_error{"not assignable"};
        }
    }

    template<class T>
    static void destroy(T& Ressource)
    {
        const auto& d{destroyer<T>()};
        assert(d);
        (*d)(Ressource);
    }

public:

    template<class T>
    static void register_type()
    {
        static bool init_once{[]()
        {
            if constexpr (std::is_default_constructible_v<T>) {
                auto& d{default_constructor<T>()};
                assert(!d);
                d = std::make_unique<DefaultConstruct<T>>();
            }

            if constexpr (std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>) {
                auto& d{assign_constructor<T>()};
                assert(!d);
                d = std::make_unique<AssignConstruct<T>>();
            }

            if constexpr (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) {
                auto& d{assigner<T>()};
                assert(!d);
                d = std::make_unique<Assign<T>>();
            }

            if constexpr (std::is_destructible_v<T>) {
                auto& d{destroyer<T>()};
                assert(!d);
                d = std::make_unique<Destroy<T>>();
            }

            return true;
        }()};
    }

    template<class T,class F>
    static void register_destroy(F&& Func)
    {
        static bool init_once{[&]()
        {
            auto& d{destroyer<T>()};
            assert(!d);
            d = std::make_unique<DestroyFunc<T,std::decay_t<F>>>(std::forward<F>(Func));
            return true;
        }()};
    }

private:

    template<class T>
    struct IConstructDestroy {
        virtual void operator()(T& Ressource) const = 0;
        virtual ~IConstructDestroy() = default;
    };

    template<class T>
    struct IAssign {
        virtual void operator()(T& Dest,T const& Source) const = 0;
        virtual void operator()(T& Dest,T&& Source) const = 0;
        virtual ~IAssign() = default;
    };

    template<class T>
    class DefaultConstruct: public IConstructDestroy<T> {
    private:
        void operator()(T& Ressource) const noexcept override
        {
            new(&Ressource) T{};
        }
    };

    template<class T>
    class AssignConstruct: public IAssign<T> {
    private:
        void operator()(T& Dest,T const& Source) const noexcept(std::is_nothrow_copy_constructible_v<T>) override
        {
            if constexpr (std::is_copy_constructible_v<T>) {
                new(&Dest) T{Source};
            } else {
                throw std::runtime_error{"not copy constructible"};
            }
        }
        void operator()(T& Dest,T&& Source) const noexcept(std::is_nothrow_move_constructible_v<T>) override
        {
            if constexpr (std::is_move_constructible_v<T>) {
                new(&Dest) T{std::move(Source)};
            } else if constexpr (std::is_copy_constructible_v<T>) {
                new(&Dest) T{Source};
            } else {
                static_assert(false,"not constructible");
            }
        }
    };

    template<class T>
    class Assign: public IAssign<T> {
    private:
        void operator()(T& Dest,T const& Source) const noexcept(std::is_nothrow_copy_assignable_v<T>) override
        {
            if constexpr (std::is_copy_assignable_v<T>) {
                Dest = Source;
            } else {
                throw std::runtime_error{"not copy assignable"};
            }
        }
        void operator()(T& Dest,T&& Source) const noexcept(std::is_nothrow_move_assignable_v<T>) override
        {
            if constexpr (std::is_move_assignable_v<T>) {
                Dest = std::move(Source);
            } else if constexpr (std::is_copy_assignable_v<T>) {
                Dest = Source;
            } else {
                static_assert(false,"not assignable");
            }
        }
    };

    template<class T>
    class Destroy: public IConstructDestroy<T> {
    private:
        void operator()(T& Ressource) const noexcept override
        {
            Ressource.~T();
        }
    };

    template<class T,class F>
    class DestroyFunc: public IConstructDestroy<T> {
    public:
        DestroyFunc() = delete;
        DestroyFunc(F const& Func)
        : mFunction{Func}
        {}
        DestroyFunc(F&& Func)
        : mFunction{std::move(Func)}
        {}
    private:
        void operator()(T& Ressource) const noexcept override
        {
            mFunction(Ressource);
        }
        F mFunction{};
    };

    template<class T>
    constexpr static std::unique_ptr<IConstructDestroy<T>>& default_constructor() noexcept
    {
        static std::unique_ptr<IConstructDestroy<T>> res{};

        return res;
    }

    template<class T>
    constexpr static std::unique_ptr<IAssign<T>>& assign_constructor() noexcept
    {
        static std::unique_ptr<IAssign<T>> res{};

        return res;
    }
    template<class T>
    constexpr static std::unique_ptr<IAssign<T>>& assigner() noexcept
    {
        static std::unique_ptr<IAssign<T>> res{};

        return res;
    }
    template<class T>
    constexpr static std::unique_ptr<IConstructDestroy<T>>& destroyer() noexcept
    {
        static std::unique_ptr<IConstructDestroy<T>> res{};

        return res;
    }

};

} //namespace phreak::detail

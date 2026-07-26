#pragma once

#include <memory>

namespace phreak::detail {

class DestroyRegistry {
public:
    template<class T>
    static void register_type()
    {
        static bool thread_safe_init{[]()
        {
            destroyer<T>() = std::unique_ptr<IDestroy>{new Destroy{[](void const* Ressource)
            {
                static_cast<T const*>(Ressource)->~T();
            }}};
            return true;
        }()};
    }

    template<class T,class F>
    static void register_destroy(F&& func)
    {
        static bool thread_safe_init{[&]()
        {
            destroyer<T>() = std::unique_ptr<IDestroy>{new Destroy{std::forward<F>(func)}};
            return true;
        }()};
    }

    template<class T>
    static void destroy(T* Ressource) noexcept
    {
        (*destroyer<T>())(Ressource);
    }
private:

    struct IDestroy {
        virtual void operator()(void* Ressource) const noexcept = 0;
        virtual ~IDestroy() = default;
    };

    template<class F>
    class Destroy: public IDestroy {
    public:
        Destroy() = delete;
        Destroy(F func)
        : mFunction{std::move(func)}
        {}
    private:
        virtual void operator()(void* Ressource) const noexcept override
        {
            mFunction(Ressource);
        }
        F mFunction{};
    };

    template<class T>
    static std::unique_ptr<IDestroy>& destroyer()
    {
        static std::unique_ptr<IDestroy> res{};

        return res;
    }
};

} //namespace phreak::detail

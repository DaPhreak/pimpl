#include "pimpl/fast_pimpl.h"
#include "pimpl/type_erasure.h"

#include <string_view>

class TestPimpl{
public:
    TestPimpl();
    TestPimpl( const std::string_view s );
    TestPimpl(TestPimpl const&);
    TestPimpl(TestPimpl&&) noexcept;
    //~TestPimpl() noexcept;
    TestPimpl& operator = (TestPimpl const&);
    TestPimpl& operator = (TestPimpl&&) noexcept;
    TestPimpl& operator = ( const std::string_view s );

    operator std::string_view () const noexcept;

    std::string_view get() const { return operator std::string_view(); }

private:
    class Impl;
    using Impl_t = phreak::pimpl_holder<phreak::detail::fixed_provider<Impl,40,8,phreak::detail::type_erasure>>;
    Impl_t mImpl;
};


[[nodiscard]] inline bool operator == (TestPimpl const& a, TestPimpl const& b)
{
    return a.get() == b.get();
}

[[nodiscard]] inline bool operator == (TestPimpl const& a, std::string_view const& b)
{
    return a.get() == b;
}

[[nodiscard]] inline bool operator == (std::string_view const& a, TestPimpl const& b)
{
    return a == b.get();
}

[[nodiscard]] inline bool operator != (TestPimpl const& a, TestPimpl const& b)
{
    return a.get() != b.get();
}

[[nodiscard]] inline bool operator != (TestPimpl const& a, std::string_view const& b)
{
    return a.get() != b;
}

[[nodiscard]] inline bool operator != (std::string_view const& a, TestPimpl const& b)
{
    return a != b.get();
}

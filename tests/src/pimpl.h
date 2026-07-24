#include "pimpl/fast_pimpl.h"

#include <string_view>

class TestPimpl{
public:
    TestPimpl();
    TestPimpl( const std::string_view s );
    TestPimpl(TestPimpl const&);
    TestPimpl(TestPimpl&&) noexcept;
    ~TestPimpl() noexcept;
    TestPimpl& operator = (TestPimpl const&);
    TestPimpl& operator = (TestPimpl&&) noexcept;
    TestPimpl& operator = ( const std::string_view s );

    operator std::string_view () const noexcept;

    std::string_view get() const { return operator std::string_view(); }

private:
    class Impl;
    phreak::fast_pimpl<Impl,48> mImpl;
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

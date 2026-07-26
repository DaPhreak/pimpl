#include "pimpl.h"

#include <string>

class TestPimpl::Impl{
public:
    Impl() = default;
    Impl( const std::string_view s )
    : mString{ s } {}
    Impl& operator = ( const std::string_view s )
    {
        mString = s;
        return *this;
    }
    operator std::string_view () const noexcept { return mString; };
private:
    std::string mString;
};

TestPimpl::TestPimpl() = default;
TestPimpl::TestPimpl(TestPimpl const&) = default;
TestPimpl::TestPimpl(TestPimpl&&) noexcept = default;
TestPimpl::~TestPimpl() noexcept = default;
TestPimpl& TestPimpl::operator = (TestPimpl const&) = default;
TestPimpl& TestPimpl::operator = (TestPimpl&&) noexcept = default;

TestPimpl::TestPimpl( const std::string_view s )
: mImpl{ s }
{}

TestPimpl& TestPimpl::operator = ( const std::string_view s )
{
    mImpl = s;
    return *this;
}

TestPimpl::operator std::string_view () const noexcept
{
    return *mImpl;
}

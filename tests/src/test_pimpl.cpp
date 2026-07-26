#include "pimpl/pimpl.h"
#include "pimpl/fast_pimpl.h"
#include "pimpl.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

namespace {

template<class T>
void test(T& s)
{
    REQUIRE( s == "ss" );

    s = "x";

    REQUIRE( s == "x" );

    {
        T cpy{ s };

        REQUIRE( s == cpy );

        cpy = std::move( s );

        REQUIRE( cpy == "x" );

        REQUIRE( s == "" ); // maybe (moved)...

        std::swap( s, cpy );
    }

    REQUIRE( s == "x" );

    {
        T cpy{ std::move( s ) };

        REQUIRE( cpy == "x" );

        REQUIRE( s == "" ); // maybe (moved)...

        std::swap( s, cpy );
    }

    REQUIRE( s == "x" );
}

TEST_CASE("Test fast pimpl", "[fast_pimpl]")
{
    using T = phreak::fast_pimpl<std::string,40>;

    T s{2,'s'};

    test(s);
}

TEST_CASE("Test pimpl", "[fast_pimpl]")
{
    using T = phreak::pimpl<std::string>;

    T s{2,'s'};

    test(s);
}

TEST_CASE("Test TestPimpl", "[fast_pimpl]")
{
    using T = TestPimpl;

    T s{"ss"};

    test(s);
}


} // ::
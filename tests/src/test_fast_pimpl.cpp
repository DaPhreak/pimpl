#include "fast_pimpl/fast_pimpl.h"
#include "pimpl.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <numeric>
#include <algorithm>

namespace {

TEST_CASE("Test fast pimpl", "[fast_pimpl]")
{
    using T = phreak::fast_pimpl<std::string,64>;

    T s{"s"};

    REQUIRE( *s == "s" );

    s = "x";

    REQUIRE( *s == "x" );

    {
        T cpy{ s }; 

        REQUIRE( *s == *cpy );

        cpy = std::move( s );

        REQUIRE( *cpy == "x" );

        REQUIRE( s->empty() ); // maybe (moved)...
    }
}

TEST_CASE("Test pimpl", "[fast_pimpl]")
{
    using T = TestPimpl;

    T s{"s"};

    REQUIRE( s == "s" );

    s = "x";

    REQUIRE( s == "x" );

    {
        T cpy{ s }; 

        REQUIRE( s == cpy );

        cpy = std::move( s );

        REQUIRE( cpy == "x" );

        REQUIRE( s.get().empty() ); // maybe (moved)...
    }
}


} // ::
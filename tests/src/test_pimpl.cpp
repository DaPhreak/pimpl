#include "pimpl/pimpl.h"
#include "pimpl/fast_pimpl.h"
#include "pimpl.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <memory_resource>

namespace {

constexpr std::string_view assigned{ "Hello, this will be on the heap!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"};

template<class T>
void test(T& s)
{
    REQUIRE( s == "ss" );

    s = assigned;

    REQUIRE(( s == assigned ));

    {
        T cpy{ s };

        REQUIRE( s == cpy );

        cpy = std::move( s );

        REQUIRE(( cpy == assigned ));

        REQUIRE( s == "" ); // maybe (moved)...

        std::swap( s, cpy );
    }

    REQUIRE(( s == assigned ));

    {
        T cpy{ std::move( s ) };

        REQUIRE(( cpy == assigned ));

        REQUIRE( s == "" ); // maybe (moved)...

        std::swap( s, cpy );
    }

    REQUIRE(( s == assigned ));
}

TEST_CASE("Test fast pimpl", "[pimpl]")
{
    using T = phreak::fast_pimpl<std::string,40>;

    T s{2,'s'};

    test(s);
}

TEST_CASE("Test pimpl", "[pimpl]")
{
    using T = phreak::pimpl<std::string>;

    T s{2,'s'};

    test(s);
}

TEST_CASE("Test pmr pimpl", "[pimpl]")
{
    using T = phreak::pimpl<std::string,std::pmr::polymorphic_allocator<std::string>>;
    std::pmr::synchronized_pool_resource mem{};

    T s{&mem,2,'s'};

    test(s);
}

TEST_CASE("Test TestPimpl", "[pimpl]")
{
    using T = TestPimpl;

    T s{"ss"};

    test(s);
}

} // ::

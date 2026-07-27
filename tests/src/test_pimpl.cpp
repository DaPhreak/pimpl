#include "pimpl/pimpl.h"
#include "pimpl/fast_pimpl.h"
#include "pimpl/type_erasure.h"
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

    static_assert(sizeof(T) == 40);

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

    T s{phreak::provider_arg,&mem,2,'s'};

    test(s);
}

TEST_CASE("Test TestPimpl", "[pimpl]")
{
    using T = TestPimpl;

    T s{"ss"};

    test(s);
}

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator = (NoCopy const&) = delete;

    int value{};
};

struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly const&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator = (MoveOnly const&) = delete;
    MoveOnly& operator = (MoveOnly&&) noexcept = default;

    int value{};
};

struct CopyOnly {
    CopyOnly() = default;
    CopyOnly(CopyOnly const&) noexcept = default;
    CopyOnly(CopyOnly&&) = delete;
    CopyOnly& operator = (CopyOnly const&) noexcept = default;
    CopyOnly& operator = (CopyOnly&&) = delete;

    int value{};
};

TEST_CASE("Test type_erasure", "[pimpl]")
{
    phreak::detail::type_erasure::register_type<NoCopy>();
    phreak::detail::type_erasure::register_type<MoveOnly>();
    phreak::detail::type_erasure::register_type<CopyOnly>();

    {
        using T = NoCopy;
        T d;

        phreak::detail::type_erasure::register_type<T>();

        REQUIRE_THROWS(phreak::detail::type_erasure::construct(T{},d));
        REQUIRE_THROWS(phreak::detail::type_erasure::assign(T{},d));
        REQUIRE_THROWS(phreak::detail::type_erasure::construct(d,T{123}));
        REQUIRE( d.value == 0 );
        d.value = 0;
        REQUIRE_THROWS(phreak::detail::type_erasure::assign(d,T{123}));
        REQUIRE( d.value == 0 );
    }

    {
        using T = MoveOnly;
        T d;

        phreak::detail::type_erasure::register_type<T>();

        REQUIRE_THROWS(phreak::detail::type_erasure::construct(T{},d));
        REQUIRE_THROWS(phreak::detail::type_erasure::assign(T{},d));

        phreak::detail::type_erasure::construct(d,T{123});
        REQUIRE( d.value == 123 );
        d.value = 0;
        phreak::detail::type_erasure::assign(d,T{123});
        REQUIRE( d.value == 123 );
    }
    {
        using T = CopyOnly;

        T d{};

        phreak::detail::type_erasure::register_type<T>();

        phreak::detail::type_erasure::construct(d,T{123});
        REQUIRE( d.value == 123 );
        d.value = 0;
        phreak::detail::type_erasure::assign(d,T{123});
        REQUIRE( d.value == 123 );
    }

    {
        using T = int;

        T d{};

        phreak::detail::type_erasure::register_type<T>();

        phreak::detail::type_erasure::construct(d,T{123});
        REQUIRE( d == 123 );
        d = 0;
        phreak::detail::type_erasure::assign(d,T{123});
        REQUIRE( d == 123 );
    }
}

} // ::

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

// Test that header file is self-contained.
#include <boost/http_proto/bnf/chunk_ext.hpp>

#include <boost/http_proto/bnf/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "test_bnf.hpp"

namespace boost {
namespace http_proto {
namespace bnf {

BOOST_STATIC_ASSERT(
    is_element<chunk_ext_elem>::value);

BOOST_STATIC_ASSERT(
    is_list<chunk_ext>::value);

class chunk_ext_test
{
public:
    void
    run()
    {
        using T = chunk_ext_elem;
        test::bad<T>( "");
        test::bad<T>( ",");
        test::bad<T>( ";");
        test::good<T>(";a");
        test::good<T>(";ab");
        test::good<T>(";ab=c");
        test::good<T>(";ab=cd");
        test::good<T>(";ab=\"cd\"");
    }
};

TEST_SUITE(chunk_ext_test, "boost.http_proto.chunk_ext");

} // bnf
} // http_proto
} // boost
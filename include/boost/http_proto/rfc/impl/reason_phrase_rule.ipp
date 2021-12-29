//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_HTTP_PROTO_RFC_IMPL_REASON_PHRASE_RULE_PP
#define BOOST_HTTP_PROTO_RFC_IMPL_REASON_PHRASE_RULE_PP

#include <boost/http_proto/rfc/reason_phrase_rule.hpp>

namespace boost {
namespace http_proto {

bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    reason_phrase_rule& t) noexcept
{
    ec = {};

    // reason-phrase   = *( HTAB / SP / VCHAR / obs-text )
    using grammar::parse;
    struct reason_chars_t
    {
        constexpr
        bool
        operator()(char c) const noexcept
        {
            return
                (static_cast<unsigned char>(
                    c) >= 32 && c != 128) ||
                (c == 9);
        }
    };
    static constexpr reason_chars_t cs{};
    auto const start = it;
    it = grammar::find_if_not(it, end, cs);
    t.s = string_view(start, it - start);
    return true;
}

} // http_proto
} // boost

#endif
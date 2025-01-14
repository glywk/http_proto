//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_HTTP_PROTO_REQUEST_HPP
#define BOOST_HTTP_PROTO_REQUEST_HPP

#include <boost/http_proto/detail/config.hpp>
#include <boost/http_proto/header_info.hpp>
#include <boost/http_proto/fields_base.hpp>
#include <boost/http_proto/method.hpp>
#include <boost/http_proto/version.hpp>

namespace boost {
namespace http_proto {

#ifndef BOOST_HTTP_PROTO_DOCS
class request_view;
#endif

/** Container for HTTP requests
*/
class BOOST_SYMBOL_VISIBLE
    request
    : public fields_base
{
    http_proto::method method_;
    http_proto::version version_;
    std::size_t method_len_;
    std::size_t target_len_;

public:
    /** Constructor
    */
    BOOST_HTTP_PROTO_DECL
    request();

    /** Constructor
    */
    BOOST_HTTP_PROTO_DECL
    request(request const&);

    /** Constructor

        The moved-from object will be
        left in the default-constructed
        state.
    */
    BOOST_HTTP_PROTO_DECL
    request(request&&) noexcept;

    /** Assignment
    */
    BOOST_HTTP_PROTO_DECL
    request&
    operator=(request&&) noexcept;

    /** Assignment
    */
    BOOST_HTTP_PROTO_DECL
    request&
    operator=(request const&);

    /** Assignment
    */
    BOOST_HTTP_PROTO_DECL
    request&
    operator=(request_view const&);

    /** Constructor
    */
    BOOST_HTTP_PROTO_DECL
    request(
        request_view const&);

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the method of this request as a known-method enum

        If the method returned is equal to
        @ref method::unknown, the method may
        be obtained as a string instead, by
        calling @ref method_str.
    */
    http_proto::method
    method() const noexcept
    {
        return method_;
    }

    /** Return the method of this request as a string
    */
    string_view
    method_str() const noexcept
    {
        return string_view(
            cbuf_, method_len_);
    }

    /** Return the request-target
    */
    string_view
    target() const noexcept
    {
        return string_view(
            cbuf_ + method_len_ + 1,
                target_len_);
    }

    /** Return the HTTP version of this request
    */
    http_proto::version
    version() const noexcept
    {
        return version_;
    }

    /** Return a read-only view to the request
    */
    BOOST_HTTP_PROTO_DECL
    operator
    request_view() const noexcept;

    /** Return serialization information
    */
    BOOST_HTTP_PROTO_DECL
    operator
    header_info() const noexcept;

    //--------------------------------------------
    //
    // Modifiers
    //
    //--------------------------------------------

    /** Clear the contents, but not the capacity
    */
    BOOST_HTTP_PROTO_DECL
    void
    clear() noexcept;

    /** Set the method of the request to the enum
    */
    void
    set_method(
        http_proto::method m)
    {
        set_impl(
            m,
            to_string(m),
            target(),
            version());
    }

    /** Set the method of the request to the string
    */
    void
    set_method(string_view s)
    {
        set_impl(
            string_to_method(s),
            s,
            target(),
            version());
    }

    /** Set the target string of the request

        This function sets the request-target.
        The caller is responsible for ensuring
        that the string passed is syntactically
        valid.
    */
    void
    set_target(string_view s)
    {
        set_impl(
            method_,
            method_str(),
            s,
            version());
    }

    /** Set the HTTP version of the request
    */
    void
    set_version(
        http_proto::version v)
    {
        set_impl(
            method_,
            method_str(),
            target(),
            v);
    }

    /** Set the method, target, and version of the request

        This is more efficient than setting the
        properties individually.
    */
    void
    set_start_line(
        http_proto::method m,
        string_view t,
        http_proto::version v =
            http_proto::version::http_1_1)
    {
        set_impl(m, to_string(m), t, v);
    }

    /** Set the method, target, and version of the request

        This is more efficient than setting the
        properties individually.
    */
    void
    set_start_line(
        string_view m,
        string_view t,
        http_proto::version v =
            http_proto::version::http_1_1)
    {
        set_impl(string_to_method(m), m, t, v);
    }

    /** Swap this with another instance
    */
    BOOST_HTTP_PROTO_DECL
    void
    swap(request& other) noexcept;

    /** Swap two instances
    */
    friend
    void
    swap(
        request& v1,
        request& v2) noexcept
    {
        v1.swap(v2);
    }

private:
    BOOST_HTTP_PROTO_DECL
    void
    set_impl(
        http_proto::method m,
        string_view ms,
        string_view t,
        http_proto::version v);
};

} // http_proto
} // boost

#endif

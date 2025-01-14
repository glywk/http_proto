//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_HTTP_PROTO_BASIC_PARSER_HPP
#define BOOST_HTTP_PROTO_BASIC_PARSER_HPP

#include <boost/http_proto/detail/config.hpp>
#include <boost/http_proto/buffer.hpp>
#include <boost/http_proto/error.hpp>
#include <boost/http_proto/string_view.hpp>
#include <boost/http_proto/bnf/range.hpp>
#include <boost/http_proto/rfc/chunk_ext_rule.hpp>
#include <boost/http_proto/rfc/field_rule.hpp>
#include <boost/url/grammar/range.hpp>
#include <boost/optional.hpp>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace boost {
namespace http_proto {

#ifndef BOOST_HTTP_PROTO_DOCS
class context;
enum class version : char;
#endif

struct chunk_info
{
    chunk_info() = default;

    std::uint64_t size; // of this chunk
    bnf::range<
        chunk_ext_rule> ext; // chunk extensions (can be empty)
    grammar::range<field_rule> trailer;
    bool fresh;         // true if this is a fresh chunk
};

/** A parser for HTTP/1 messages.

    The parser is strict. Any malformed
    inputs according to the documented
    HTTP ABNFs is treated as an
    unrecoverable error.
*/
class BOOST_SYMBOL_VISIBLE
    parser
{
BOOST_HTTP_PROTO_PROTECTED:

    enum class state
    {
        start_line,
        header_fields,
        body,
        end_of_message,
        end_of_stream
    };

    // applies to all messages
    struct config
    {
        config() noexcept;

        std::size_t header_limit;   // max header size
        optional<std::uint64_t>
            body_limit;             // max body size
    };

    // per-message state
    struct message
    {
        std::size_t count;          // number of fields
        std::size_t start_len;      // chars in start-line
        std::size_t fields_len;     // chars in fields,
                                    //   including last CRLF
        std::size_t n_chunk;        // bytes of chunk header
        std::size_t n_payload;      // bytes of body or chunk
        std::uint64_t n_remain;     // remaining body or chunk

        std::uint64_t payload_seen; // total body received
        chunk_info chunk;

        optional<std::uint64_t>
            content_len;            // value of Content-Length
        http_proto::version
            version;                // HTTP-version

        bool skip_body : 1;         // no body expected
        bool got_chunked : 1;
        bool got_close : 1;
        bool got_keep_alive : 1;
        bool got_upgrade : 1;
        bool need_eof : 1;
    };

    context& ctx_;
    char* buf_;
    std::size_t cap_;           // allocated size
    std::size_t size_;          // committed part
    std::size_t used_;          // parsed part
    state state_;

    bool got_eof_;

    config cfg_;
    message m_;

    explicit
    parser(
        context& ctx) noexcept;

public:
    BOOST_HTTP_PROTO_DECL
    ~parser();

    /** Returns true if the payload uses chunked encoding.
    */
    bool
    is_chunked() const noexcept
    {
        return m_.got_chunked;
    }

    /** Returns `true` if a complete message has been parsed.

        Calling @ref reset prepares the parser
        to process the next message in the stream.

    */
    bool
    is_end_of_message() const noexcept
    {
        return state_ ==
            state::end_of_message;
    }

    /** Returns `true` if no input remains and no more is coming.

        Calling @ref reset prepares the parser
        for additional input from a new stream.
    */
    bool
    //end_of_stream() const noexcept // ??
    is_end_of_stream() const noexcept
    {
        return state_ ==
            state::end_of_stream;
    }

    //http_proto::header_fields
    //fields() const noexcept;

    BOOST_HTTP_PROTO_DECL
    chunk_info
    chunk() const noexcept;

    BOOST_HTTP_PROTO_DECL
    string_view
    body() const noexcept;

    /** Prepare the parser for the next message.
    */
    BOOST_HTTP_PROTO_DECL
    void
    reset();

    BOOST_HTTP_PROTO_DECL
    mutable_buffer
    prepare();

    BOOST_HTTP_PROTO_DECL
    void
    commit(std::size_t n);

    BOOST_HTTP_PROTO_DECL
    void
    commit_eof();

    BOOST_HTTP_PROTO_DECL
    void
    discard_header() noexcept;

    BOOST_HTTP_PROTO_DECL
    void
    discard_body() noexcept;

    BOOST_HTTP_PROTO_DECL
    void
    discard_chunk() noexcept;

    /** Indicate that the current message has no payload.

        This informs the parser not to read a payload for
        the next message, regardless of the presence or
        absence of certain fields such as Content-Length
        or a chunked Transfer-Encoding. Depending on the
        request, some responses do not carry a body. For
        example, a 200 response to a CONNECT request from
        a tunneling proxy, or a response to a HEAD request.
        In these cases, callers may use this function inform
        the parser that no body is expected. The parser will
        consider the message complete after the header has
        been received.

        @par Preconditions

        This function must called before any calls to parse
        the current message.

        @see
            https://datatracker.ietf.org/doc/html/rfc7230#section-3.3

    */
    BOOST_HTTP_PROTO_DECL
    void
    skip_body();

    /** Parse the message header using buffered input

        If the message header is incomplete, this
        function attempts to parse buffered input
        until a complete message header is parsed.
        Otherwise, if the header is already complete
        this function does nothing.

        If there is insufficient buffered input, the
        error will be set to @ref grammar::error::incomplete.
        In this case the caller should transfer more
        input to the parser and call this function
        again.

        @param ec Set to the error, if any occurred.
    */
    BOOST_HTTP_PROTO_DECL
    void
    parse_header(error_code& ec);

    BOOST_HTTP_PROTO_DECL
    void
    parse_body(error_code& ec);

    BOOST_HTTP_PROTO_DECL
    void
    parse_chunk(
        error_code& ec);

    template<class Body>
    void
    attach_body(Body&)
    {
    }

    template<class Body>
    void
    attach_body(Body&&)
    {
    }

private:
    virtual char* parse_start_line(
        char*, char const*, error_code&) noexcept = 0;
    virtual void finish_header(error_code&) = 0;

    char* parse_fields(char*, char const*, error_code&);
    void do_connection(string_view, error_code&);
    void do_content_length(string_view, error_code&);
    void do_transfer_encoding(string_view, error_code&);
    void do_upgrade(string_view, error_code&);
};

} // http_proto
} // boost

#endif

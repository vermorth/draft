#include "reply_parser.hh"

#include <string>

#include "3rdparty/asio-1.18.1/include/asio.hpp"

namespace draft {
namespace http {

#include "3rdparty/asio-1.18.1/include/asio/yield.hpp"

ParseHelper::Result ReplyParser::consume(Reply &rep, char c) {
    reenter(this) {
        rep._headers.clear();
        rep._content.clear();
        _content_length = 0;

        // 版本号
        // HTTP protocol identifier.
        if (c != 'H')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != 'T')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != 'T')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != 'P')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        // Slash.
        if (c != '/')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        // Major version number.
        if (!ParseHelper::IsDigit(c))
            return ParseHelper::Result::bad;
        while (ParseHelper::IsDigit(c)) {
            yield return ParseHelper::Result::indeterminate;
        }
        // Dot.
        if (c != '.')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        // Minor version number.
        if (!ParseHelper::IsDigit(c))
            return ParseHelper::Result::bad;
        while (ParseHelper::IsDigit(c)) {
            yield return ParseHelper::Result::indeterminate;
        }

        // Space.
        if (c != ' ')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

        // 状态码
        rep._status = 0;
        if (!ParseHelper::IsDigit(c))
            return ParseHelper::Result::bad;
        while (ParseHelper::IsDigit(c)) {
            rep._status = rep._status * 10 + c - '0';
            yield return ParseHelper::Result::indeterminate;
        }

        // Space.
        if (c != ' ')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

        // 状态文本
        while (ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) && c != '\r') {
            yield return ParseHelper::Result::indeterminate;
        }

        // CRLF.
        if (c != '\r')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != '\n')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

        // Headers
        while ((ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
                !ParseHelper::IsTspecial(c) && c != '\r') ||
               (c == ' ' || c == '\t')) {
            if (c == ' ' || c == '\t') {
                // Leading whitespace. Must be continuation of previous header's
                // value.
                if (rep._headers.empty())
                    return ParseHelper::Result::bad;
                while (c == ' ' || c == '\t')
                    yield return ParseHelper::Result::indeterminate;
            } else {
                // Start the next header.
                rep._headers.push_back(Header());

                // Header name.
                while (ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
                       !ParseHelper::IsTspecial(c) && c != ':') {
                    rep._headers.back()._name.push_back(c);
                    yield return ParseHelper::Result::indeterminate;
                }

                // Colon and space separates the header name from the header
                // value.
                if (c != ':')
                    return ParseHelper::Result::bad;
                yield return ParseHelper::Result::indeterminate;
                if (c != ' ')
                    return ParseHelper::Result::bad;
                yield return ParseHelper::Result::indeterminate;
            }

            // Header value.
            while (ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
                   c != '\r') {
                rep._headers.back()._value.push_back(c);
                yield return ParseHelper::Result::indeterminate;
            }

            // CRLF.
            if (c != '\r')
                return ParseHelper::Result::bad;
            yield return ParseHelper::Result::indeterminate;
            if (c != '\n')
                return ParseHelper::Result::bad;
            yield return ParseHelper::Result::indeterminate;
        }

        // CRLF.
        if (c != '\r')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != '\n')
            return ParseHelper::Result::bad;

        // Check for optional Content-Length header.
        for (std::size_t i = 0; i < rep._headers.size(); ++i) {
            if (ParseHelper::IsHeadersEqual(
                    rep._headers[i]._name, ParseHelper::_content_length_name)) {
                try {
                    _content_length =
                        static_cast<size_t>(std::stol(rep._headers[i]._value));
                } catch (std::invalid_argument &e) {
                    return ParseHelper::Result::bad;
                }
            }
        }

        // Content.
        while (rep._content.size() < _content_length) {
            yield return ParseHelper::Result::indeterminate;
            rep._content.push_back(c);
        }
    }

    return ParseHelper::Result::good;
}

#include "3rdparty/asio-1.18.1/include/asio/unyield.hpp"

}  // namespace http
}  // namespace draft
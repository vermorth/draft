#include "request_parser.hh"

#include <string>

#include "3rdparty/asio-1.18.1/include/asio.hpp"

namespace draft {
namespace http {

#include "3rdparty/asio-1.18.1/include/asio/yield.hpp"

ParseHelper::Result RequestParser::consume(Request &req, char c) {
    reenter(this) {
        req._method.clear();
        req._url.clear();
        req._headers.clear();
        req._content.clear();
        _content_length = 0;

        // Request method.
        while (ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
               !ParseHelper::IsTspecial(c) && c != ' ') {
            req._method.push_back(c);
            yield return ParseHelper::Result::indeterminate;
        }
        if (req._method.empty())
            return ParseHelper::Result::bad;

        // Space.
        if (c != ' ')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

        // URL.
        while (!ParseHelper::IsCtl(c) && c != ' ') {
            req._url.push_back(c);
            yield return ParseHelper::Result::indeterminate;
        }
        if (req._url.empty())
            return ParseHelper::Result::bad;

        // Path & Query.
        _qm_index = req._url.find_first_of("?");
        if (_qm_index != std::string::npos) {
            // Path
            req._path = req._url.substr(0, _qm_index);
            // Query
            req._query = req._url.substr(_qm_index + 1);
        } else {
            req._path = req._url;
        }

        // Space.
        if (c != ' ')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

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

        // CRLF.
        if (c != '\r')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;
        if (c != '\n')
            return ParseHelper::Result::bad;
        yield return ParseHelper::Result::indeterminate;

        // Headers.
        while ((ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
                !ParseHelper::IsTspecial(c) && c != '\r') ||
               (c == ' ' || c == '\t')) {
            if (c == ' ' || c == '\t') {
                // Leading whitespace. Must be continuation of previous header's
                // value.
                if (req._headers.empty())
                    return ParseHelper::Result::bad;
                while (c == ' ' || c == '\t')
                    yield return ParseHelper::Result::indeterminate;
            } else {
                // Start the next header.
                req._headers.push_back(Header());

                // Header name.
                while (ParseHelper::IsChar(c) && !ParseHelper::IsCtl(c) &&
                       !ParseHelper::IsTspecial(c) && c != ':') {
                    req._headers.back()._name.push_back(c);
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
                req._headers.back()._value.push_back(c);
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
        for (std::size_t i = 0; i < req._headers.size(); ++i) {
            if (ParseHelper::IsHeadersEqual(
                    req._headers[i]._name, ParseHelper::_content_length_name)) {
                try {
                    _content_length =
                        static_cast<size_t>(std::stol(req._headers[i]._value));
                } catch (std::invalid_argument &e) {
                    return ParseHelper::Result::bad;
                }
            }
        }

        // Content.
        while (req._content.size() < _content_length) {
            yield return ParseHelper::Result::indeterminate;
            req._content.push_back(c);
        }
    }

    return ParseHelper::Result::good;
}
#include "3rdparty/asio-1.18.1/include/asio/unyield.hpp"

}  // namespace http
}  // namespace draft
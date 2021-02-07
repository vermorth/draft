#include "request.hh"

#include <sstream>
#include <string>
#include <tuple>

namespace draft {
namespace http {

// 参考RFC 3986
bool shouldEscape(char c, EscapeMode mode) {
    // §2.3 Unreserved characters (alphanum)
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')) {
        return false;
    }

    switch (c) {
    // §2.3 Unreserved characters (mark)
    case '-':
    case '_':
    case '.':
    case '~':
        return false;

    // §2.2 Reserved characters (reserved)
    case '$':
    case '&':
    case '+':
    case ',':
    case '/':
    case ':':
    case ';':
    case '=':
    case '?':
    case '@':
        // Different sections of the URL allow a few of
        // the reserved characters to appear unescaped.
        if (mode == EscapeMode::url) {
            // §3.3
            // The RFC allows : @ & = + $ but saves / ; , for assigning
            // meaning to individual path segments. This package
            // only manipulates the path as a whole, so we allow those
            // last three as well. That leaves only ? to escape.
            return c == '?';
        } else if (mode == EscapeMode::query) {
            // The RFC reserves (so we must escape) everything.
            return true;
        }
    }

    // Everything else must be escaped.
    return true;
}

std::tuple<std::string, bool> Request::escape(const std::string &in,
                                              EscapeMode mode) {
    int space_count(0), hex_count(0);
    for (auto c : in) {
        if (shouldEscape(c, mode)) {
            if (c == ' ' && mode == EscapeMode::query) {
                // 如果是query模式，空格被编码为 '+'
                space_count++;
            } else {
                // 如果是path模式，空格被编码为 '%20'
                hex_count++;
            }
        }
    }

    if (space_count == 0 && hex_count == 0) {
        // 没有要编码的字符，直接返回
        return std::make_tuple(in, true);
    }

    if (hex_count == 0) {
        // 仅有空格要替换为'+'
        std::string out(in);
        for (auto &c : out) {
            if (c == ' ') {
                c = '+';
            }
        }
        return std::make_tuple(out, true);
    }

    char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::stringstream ss;
    for (auto c : in) {
        if (c == ' ' && mode == EscapeMode::query) {
            ss << '+';
        } else if (shouldEscape(c, mode)) {
            ss << '%';
            ss << hex_chars[c >> 4];
            ss << hex_chars[c & 0x0F];
        } else {
            ss << c;
        }
    }
    return std::make_tuple(ss.str(), true);
}

std::tuple<std::string, bool> Request::unescape(const std::string &in,
                                                EscapeMode mode) {
    std::string out;
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return std::make_tuple("", false);
                }
            } else {
                return std::make_tuple("", false);
            }
        } else if (in[i] == '+') {
            if (mode == EscapeMode::query) {
                out += ' ';
            } else {
                out += '+';
            }
        } else {
            out += in[i];
        }
    }
    return std::make_tuple(out, true);
}

std::tuple<std::string, bool> Request::UrlDecode(const std::string &in) {
    return unescape(in, EscapeMode::url);
}

std::tuple<std::string, bool> Request::QueryDecode(const std::string &in) {
    return unescape(in, EscapeMode::query);
}

std::tuple<std::string, bool> Request::UrlEncode(const std::string &in) {
    return escape(in, EscapeMode::url);
}

std::tuple<std::string, bool> Request::QueryEncode(const std::string &in) {
    return escape(in, EscapeMode::query);
}

namespace misc_strings {
const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};
const char http_version[] = {'H', 'T', 'T', 'P', '/', '1', '.', '1'};
const char space[] = {' '};
const char question[] = {'?'};
const char with[] = {'&'};
const char equal[] = {'='};
}  // namespace misc_strings

std::vector<asio::const_buffer> Request::ToBuffers() {
    std::vector<asio::const_buffer> buffers;
    // 请求行
    buffers.push_back(asio::buffer(_method));
    buffers.push_back(asio::buffer(misc_strings::space));
    buffers.push_back(asio::buffer(_path));
    if (!_query_params.empty()) {
        int idx = 0;
        for (auto &p : _query_params) {
            if (idx == 0) {
                buffers.push_back(asio::buffer(misc_strings::question));
            } else {
                buffers.push_back(asio::buffer(misc_strings::with));
            }
            buffers.push_back(asio::buffer(p.first));
            buffers.push_back(asio::buffer(misc_strings::equal));
            buffers.push_back(asio::buffer(p.second));
            ++idx;
        }
    }
    buffers.push_back(asio::buffer(misc_strings::space));
    buffers.push_back(asio::buffer(misc_strings::http_version));
    buffers.push_back(asio::buffer(misc_strings::crlf));
    // 请求头
    for (std::size_t i = 0; i < _headers.size(); ++i) {
        Header &h = _headers[i];
        buffers.push_back(asio::buffer(h._name));
        buffers.push_back(asio::buffer(misc_strings::name_value_separator));
        buffers.push_back(asio::buffer(h._value));
        buffers.push_back(asio::buffer(misc_strings::crlf));
    }
    buffers.push_back(asio::buffer(misc_strings::crlf));
    // 请求体
    buffers.push_back(asio::buffer(_content));
    return buffers;
}

}  // namespace http
}  // namespace draft
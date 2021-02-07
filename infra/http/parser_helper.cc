#include "parser_helper.hh"

#include <algorithm>
#include <string>

namespace draft {
namespace http {

const char *const ParseHelper::_content_length_name = "Content-Length";

bool ParseHelper::IsChar(int c) { return c >= 0 && c <= 127; }

bool ParseHelper::IsCtl(int c) { return (c >= 0 && c <= 31) || (c == 127); }

bool ParseHelper::IsTspecial(int c) {
    switch (c) {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
        return true;
    default:
        return false;
    }
}

bool ParseHelper::IsDigit(int c) { return c >= '0' && c <= '9'; }

bool ParseHelper::IsHeadersEqual(const std::string &a, const std::string &b) {
    if (a.length() != b.length())
        return false;
    return std::equal(a.begin(), a.end(), b.begin(),
                      [](char a, char b) -> bool {
                          return std::tolower(a) == std::tolower(b);
                      });
}

}  // namespace http
}  // namespace draft
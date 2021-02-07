#pragma once

#include <string>

namespace draft {
namespace http {

class ParseHelper {
 public:
    // 用于表示解析状态
    enum class Result { good, bad, indeterminate };

    // The name of the content length header.
    static const char *const _content_length_name;

    // Check if a byte is an HTTP character.
    static bool IsChar(int c);
    // Check if a byte is an HTTP control character.
    static bool IsCtl(int c);
    // Check if a byte is defined as an HTTP tspecial character.
    static bool IsTspecial(int c);
    // Check if a byte is a digit.
    static bool IsDigit(int c);
    // Check whether the two request header names match.
    static bool IsHeadersEqual(const std::string &a, const std::string &b);
};

}  // namespace http
}  // namespace draft
#pragma once

#include <string>
#include <tuple>

#include "3rdparty/asio-1.18.1/include/asio.hpp"

#include "parser_helper.hh"
#include "request.hh"

namespace draft {
namespace http {

class RequestParser : asio::coroutine {
 public:
    // 从begin ~ end对应的char序列中解析http请求到request。
    // 如果整个请求都解析完成，返回的result为good,
    // 如果解析出错，返回的result为bad,
    // 返回indeterminate表示数据不完整，需要更多数据。 返回的InputIterator
    // begin表示当前解析消耗了多少数据。
    template <typename InputIterator>
    std::tuple<ParseHelper::Result, InputIterator>
    Parse(Request &req, InputIterator begin, InputIterator end) {
        while (begin != end) {
            ParseHelper::Result result = consume(req, *begin++);
            if (result == ParseHelper::Result::good ||
                result == ParseHelper::Result::bad)
                return std::make_tuple(result, begin);
        }
        return std::make_tuple(ParseHelper::Result::indeterminate, begin);
    }

 private:
    // Handle the next character of input.
    ParseHelper::Result consume(Request &req, char input);

 private:
    // Content length as decoded from headers. Defaults to 0.
    std::size_t _content_length;
    // ? index in url
    std::size_t _qm_index;
};

}  // namespace http
}  // namespace draft
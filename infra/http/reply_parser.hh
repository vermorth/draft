#pragma once

#include <string>
#include <tuple>

#include "3rdparty/asio-1.18.1/include/asio.hpp"

#include "reply.hh"
#include "parser_helper.hh"

namespace draft {
namespace http {
class ReplyParser : asio::coroutine {
 public:
    template <typename InputIterator>
    std::tuple<ParseHelper::Result, InputIterator>
    Parse(Reply &rep, InputIterator begin, InputIterator end) {
        while (begin != end) {
            ParseHelper::Result result = consume(rep, *begin++);
            if (result == ParseHelper::Result::good ||
                result == ParseHelper::Result::bad)
                return std::make_tuple(result, begin);
        }
        return std::make_tuple(ParseHelper::Result::indeterminate, begin);
    }

 private:
    ParseHelper::Result consume(Reply &rep, char input);

 private:
    std::size_t _content_length;
};

}  // namespace http
}  // namespace draft
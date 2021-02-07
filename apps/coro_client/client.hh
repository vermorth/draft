#pragma once

#include <memory>
#include <string>

#include "infra/http/reply.hh"
#include "infra/http/request.hh"
#include "infra/http/reply_parser.hh"

namespace draft {
namespace http {
namespace coro_client {

class Context {
 public:
    asio::ip::tcp::socket _socket;      // 连接用的socket
    asio::ip::tcp::resolver _resolver;  // 域名解析
    std::array<char, 8192> _buffer;     // 读取buffer
    Request _request;                   // 要发送的请求
    Reply _reply;                       // 请求的响应
    ReplyParser _reply_parser;          // 响应解析
    ParseHelper::Result _valid_reply;   // 标识reply解析是否正常

    Context(asio::io_context &io_ctx)
        : _socket(io_ctx), _resolver(io_ctx),
          _valid_reply(ParseHelper::Result::indeterminate) {}

    void SetRequest(const Request &request) {
        _request = request;
    }
};

void Do(std::shared_ptr<Context> client_ctx,
        asio::coroutine coro = asio::coroutine(),
        asio::error_code ec = asio::error_code(), std::size_t length = 0,
        asio::ip::tcp::resolver::results_type results = {});

}  // namespace coro_client
}  // namespace http
}  // namespace draft
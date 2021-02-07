#include "client.hh"

#include <functional>
#include <iostream>

namespace draft {
namespace http {
namespace coro_client {

#include "3rdparty/asio-1.18.1/include/asio/yield.hpp"

void Do(std::shared_ptr<Context> client_ctx, asio::coroutine coro,
        asio::error_code ec, std::size_t length,
        asio::ip::tcp::resolver::results_type results) {
    if (ec && ec != asio::error::eof) {
        std::cout << "Error:\n" << ec.message() << std::endl;
        return;
    }

    auto gen_io_func = [&]() {
        return std::bind(&Do, client_ctx, coro, std::placeholders::_1,
                         std::placeholders::_2,
                         asio::ip::tcp::resolver::results_type());
    };

    reenter(coro) {
        yield {
            auto result_func =
                std::bind(&Do, client_ctx, coro, std::placeholders::_1, 0,
                          std::placeholders::_2);
            client_ctx->_resolver.async_resolve(client_ctx->_request._host,
                                                client_ctx->_request._port,
                                                result_func);
        }

        yield {
            auto conn_func =
                std::bind(&Do, client_ctx, coro, std::placeholders::_1, 0,
                          asio::ip::tcp::resolver::results_type());
            client_ctx->_socket.async_connect(*results, conn_func);
        }

        yield async_write(client_ctx->_socket, client_ctx->_request.ToBuffers(),
                          gen_io_func());

        client_ctx->_buffer.fill(0);

        do {
            yield client_ctx->_socket.async_read_some(
                asio::buffer(client_ctx->_buffer), gen_io_func());
            // 解析响应
            std::tie(client_ctx->_valid_reply, std::ignore) =
                client_ctx->_reply_parser.Parse(
                    client_ctx->_reply, client_ctx->_buffer.data(),
                    client_ctx->_buffer.data() + length);
        } while (client_ctx->_valid_reply ==
                 ParseHelper::Result::indeterminate);

        // 关闭连接
        client_ctx->_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        client_ctx->_socket.close(ec);

        // 解析响应出错处理
        if (client_ctx->_valid_reply != ParseHelper::Result::good) {
            std::cout << "Error:\n"
                      << "error occurred while parsing reply" << std::endl;
            yield break;
        }

        std::cout << "Status:\n" << client_ctx->_reply._status << '\n';
        if (client_ctx->_reply._headers.size() > 0) {
            std::cout << "Header:\n";
            for (auto h : client_ctx->_reply._headers) {
                std::cout << "  " << h._name << ": " << h._value << '\n';
            }
        }
        std::cout << "Body:\n" << client_ctx->_reply._content << std::endl;
    }
}
#include "3rdparty/asio-1.18.1/include/asio/unyield.hpp"

}  // namespace coro_client
}  // namespace http
}  // namespace draft
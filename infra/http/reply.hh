#pragma once

#include <vector>

#include "3rdparty/asio-1.18.1/include/asio.hpp"
#include "header.hh"

namespace draft {
namespace http {

enum class StatusCode : int {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
};

// 用于将StatusCode转换为底层的int类型
template <typename E>
constexpr std::underlying_type_t<E> ToUnderType(E enumerator) noexcept {
    return static_cast<std::underlying_type_t<E>>(enumerator);
}

struct Reply {
    // http状态码
    int _status;
    // 响应头部
    std::vector<Header> _headers;
    // 响应体
    std::string _content;

    // 将reply转换为buffer数组。buffers并不拥有底层内存，所以reply对象在响应发送给用户之前
    // 不能被释放
    std::vector<asio::const_buffer> ToBuffers();

    // 预设的响应
    static Reply PresetReply(StatusCode status);
};

}  // namespace http
}  // namespace draft
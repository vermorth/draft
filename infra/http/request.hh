#pragma once

#include <map>
#include <string>
#include <vector>

#include "3rdparty/asio-1.18.1/include/asio.hpp"
#include "header.hh"

namespace draft {
namespace http {
// url 表示请求中 POST /req/url?a=233 HTTP/1.1 的第二部分
// query 表示 url中的参数名和参数值，比如 a 和 233
enum class EscapeMode { url, query };

// http请求
//
// POST /req/url?a=233 HTTP/1.1
// Host: www.example.com
// Content-Length: 233
//
// Body
struct Request {
    // 请求方法
    // [For Client][For Server]
    std::string _method;
    // 请求域名（ip）和 端口
    // [For Client]
    std::string _host;
    std::string _port;
    // 请求url，包含path和query两部分，encode之后的
    // [For Server]
    std::string _url;
    // 请求path，encode之后的
    // [For Client][For Server]
    std::string _path;
    // 请求query，encode之后的
    // [For Server]
    std::string _query;
    // 请求query params，encode之前的
    // 在request parser阶段不会填充该字段，延迟到router serve阶段填充
    // [For Client][For Server]
    std::map<std::string, std::string> _query_params;
    // 请求header
    // [For Client][For Server]
    std::vector<Header> _headers;
    // 请求体
    // [For Client][For Server]
    std::string _content;

 public:
    std::vector<asio::const_buffer> ToBuffers();

    // 执行url decode，不会解码'+'
    static std::tuple<std::string, bool> UrlDecode(const std::string &in);
    static std::tuple<std::string, bool> UrlEncode(const std::string &in);
    // 执行query decode，'+'会被替换为' '
    static std::tuple<std::string, bool> QueryDecode(const std::string &in);
    static std::tuple<std::string, bool> QueryEncode(const std::string &in);

 private:
    static std::tuple<std::string, bool> unescape(const std::string &in,
                                                  EscapeMode mode);
    static std::tuple<std::string, bool> escape(const std::string &in,
                                                EscapeMode mode);
};

}  // namespace http
}  // namespace draft
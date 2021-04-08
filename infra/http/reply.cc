#include "reply.hh"

#include <string>

namespace draft {
namespace http {

namespace status_strings {
const std::string ok = "HTTP/1.1 200 OK\r\n";
const std::string created = "HTTP/1.1 201 Created\r\n";
const std::string accepted = "HTTP/1.1 202 Accepted\r\n";
const std::string no_content = "HTTP/1.1 204 No Content\r\n";
const std::string multiple_choices = "HTTP/1.1 300 Multiple Choices\r\n";
const std::string moved_permanently = "HTTP/1.1 301 Moved Permanently\r\n";
const std::string moved_temporarily = "HTTP/1.1 302 Moved Temporarily\r\n";
const std::string not_modified = "HTTP/1.1 304 Not Modified\r\n";
const std::string bad_request = "HTTP/1.1 400 Bad Request\r\n";
const std::string unauthorized = "HTTP/1.1 401 Unauthorized\r\n";
const std::string forbidden = "HTTP/1.1 403 Forbidden\r\n";
const std::string not_found = "HTTP/1.1 404 Not Found\r\n";
const std::string internal_server_error = "HTTP/1.1 500 Internal Server Error\r\n";
const std::string not_implemented = "HTTP/1.1 501 Not Implemented\r\n";
const std::string bad_gateway = "HTTP/1.1 502 Bad Gateway\r\n";
const std::string service_unavailable = "HTTP/1.1 503 Service Unavailable\r\n";

asio::const_buffer ToBuffers(StatusCode status) {
    switch (status) {
    case StatusCode::ok:
        return asio::buffer(ok);
    case StatusCode::created:
        return asio::buffer(created);
    case StatusCode::accepted:
        return asio::buffer(accepted);
    case StatusCode::no_content:
        return asio::buffer(no_content);
    case StatusCode::multiple_choices:
        return asio::buffer(multiple_choices);
    case StatusCode::moved_permanently:
        return asio::buffer(moved_permanently);
    case StatusCode::moved_temporarily:
        return asio::buffer(moved_temporarily);
    case StatusCode::not_modified:
        return asio::buffer(not_modified);
    case StatusCode::bad_request:
        return asio::buffer(bad_request);
    case StatusCode::unauthorized:
        return asio::buffer(unauthorized);
    case StatusCode::forbidden:
        return asio::buffer(forbidden);
    case StatusCode::not_found:
        return asio::buffer(not_found);
    case StatusCode::internal_server_error:
        return asio::buffer(internal_server_error);
    case StatusCode::not_implemented:
        return asio::buffer(not_implemented);
    case StatusCode::bad_gateway:
        return asio::buffer(bad_gateway);
    case StatusCode::service_unavailable:
        return asio::buffer(service_unavailable);
    default:
        return asio::buffer(internal_server_error);
    }
}
}  // namespace status_strings

namespace misc_strings {
const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};
}  // namespace misc_strings

namespace preset_content_strings {
const char ok[] = "";
const char created[] = "201 Created";
const char accepted[] = "202 Accepted";
const char no_content[] = "204 No Content";
const char multiple_choices[] = "300 Multiple Choices";
const char moved_permanently[] = "301 Moved Permanently";
const char moved_temporarily[] = "302 Moved Temporarily";
const char not_modified[] = "304 Not Modified";
const char bad_request[] = "400 Bad Request";
const char unauthorized[] = "401 Unauthorized";
const char forbidden[] = "403 Forbidden";
const char not_found[] = "404 Not Found";
const char internal_server_error[] = "500 Internal Server Error";
const char not_implemented[] = "501 Not Implemented";
const char bad_gateway[] = "502 Bad Gateway";
const char service_unavailable[] = "503 Service Unavailable";

std::string GetContent(StatusCode status) {
    switch (status) {
    case StatusCode::ok:
        return ok;
    case StatusCode::created:
        return created;
    case StatusCode::accepted:
        return accepted;
    case StatusCode::no_content:
        return no_content;
    case StatusCode::multiple_choices:
        return multiple_choices;
    case StatusCode::moved_permanently:
        return moved_permanently;
    case StatusCode::moved_temporarily:
        return moved_temporarily;
    case StatusCode::not_modified:
        return not_modified;
    case StatusCode::bad_request:
        return bad_request;
    case StatusCode::unauthorized:
        return unauthorized;
    case StatusCode::forbidden:
        return forbidden;
    case StatusCode::not_found:
        return not_found;
    case StatusCode::internal_server_error:
        return internal_server_error;
    case StatusCode::not_implemented:
        return not_implemented;
    case StatusCode::bad_gateway:
        return bad_gateway;
    case StatusCode::service_unavailable:
        return service_unavailable;
    default:
        return internal_server_error;
    }
}
}  // namespace preset_content_strings

std::vector<asio::const_buffer> Reply::ToBuffers() {
    std::vector<asio::const_buffer> buffers;
    buffers.push_back(
        status_strings::ToBuffers(static_cast<StatusCode>(_status)));
    for (std::size_t i = 0; i < _headers.size(); ++i) {
        Header &h = _headers[i];
        buffers.push_back(asio::buffer(h._name));
        buffers.push_back(asio::buffer(misc_strings::name_value_separator));
        buffers.push_back(asio::buffer(h._value));
        buffers.push_back(asio::buffer(misc_strings::crlf));
    }
    buffers.push_back(asio::buffer(misc_strings::crlf));
    buffers.push_back(asio::buffer(_content));
    return buffers;
}

Reply Reply::PresetReply(StatusCode status) {
    Reply rep;
    rep._status = ToUnderType(status);
    rep._content = preset_content_strings::GetContent(status);
    rep._headers.resize(2);
    rep._headers[0]._name = "Content-Length";
    rep._headers[0]._value = std::to_string(rep._content.size());
    rep._headers[1]._name = "Content-Type";
    rep._headers[1]._value = "text/plain; charset=utf-8";
    return rep;
}

}  // namespace http
}  // namespace draft
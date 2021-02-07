#pragma once

#include <string>

namespace draft {
namespace http {

struct Header {
    std::string _name;
    std::string _value;
};

}  // namespace http
}  // namespace draft
//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef UTILITIES_UTF8
#define UTILITIES_UTF8

#include <cstdint>

namespace Utilities {

class Utf8 {
    public:
    static int8_t validate(const uint8_t byte, int8_t& header_length);
    static int8_t header_length(const uint8_t byte);
            
};
}
#endif

//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_utf8.hpp>

#include <cstdint>

namespace Utilities {
    int8_t Utf8::validate(const uint8_t byte, int8_t& header_length) {
        if ( header_length == 0 ) {
            header_length = Utf8::header_length(byte);
        } else if ( header_length > 0 && (byte & 0xC0) == 0x80){ 
            --header_length; 
        }
        return header_length;
    }

    int8_t Utf8::header_length(const uint8_t byte) {
        if( (byte & 0x80) == 0x00) { return  1; }
        else if( (byte & 0xC0) == 0x80) { return -1; }
        else if( (byte & 0xD0) == 0xC0) { return  2; }
        else if( (byte & 0xF0) == 0xE0) { return  3; }
        else if( (byte & 0xF8) == 0xF0) { return  4; }
        return -1;
    }
}

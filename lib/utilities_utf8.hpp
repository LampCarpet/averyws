//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef UTILITIES_UTF8
#define UTILITIES_UTF8

#include <cstdint>

namespace Utilities {
namespace Utf8 {

class Byte{
    public:
        Byte(): length_{0},it_{0} {};
        bool validate(const uint8_t byte);
        bool valid() const;
        bool complete() const;
        uint8_t &length(){return length_;}
        int8_t  &it(){return it_;}
    private:
        bool set_length(const uint8_t byte);
        uint8_t length_;
        int8_t  it_;
        //only get/set very rarely
        uint8_t previous_byte_;
};

}
}
#endif

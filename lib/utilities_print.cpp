//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_print.hpp>

#include <cstdint>
#include <iostream>

namespace Utilities {
    void Print::hex(void *begin, uint64_t size) {
        uint8_t* byte = static_cast<uint8_t*>(begin);
        for (uint64_t i = 0; i < size; i++) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex 
                << static_cast<unsigned int>(byte[i]) << " ";
        }
    }
}

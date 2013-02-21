//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef UTILITIES_RANDOM
#define UTILITIES_RANDOM

#include <vector>
#include <cstdint>

namespace Utilities {
    class Random {
        public:
        //guaranteed to be unpredictably random. Throws an exception if unsafe
        static std::vector<uint8_t> bytes_safe(uint32_t size);
   
        //try to be safe but if it isn't, generate a pseudo-random number of bytes
        static std::vector<uint8_t> bytes_safe_attempt(uint32_t size);
    
        static std::vector<uint8_t> bytes_pseudo(uint32_t size);
    };
}
#endif

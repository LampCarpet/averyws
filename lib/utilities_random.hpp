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

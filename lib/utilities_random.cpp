//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_random.hpp>

#include <vector>
#include <cstdint>

#include <random>
#include <openssl/rand.h>

#include <stdexcept>

namespace Utilities {

    //guaranteed to be unpredictably random. Throws an exception if unsafe
    std::vector<uint8_t> Random::bytes_safe(uint32_t size)
    {
        std::vector<uint8_t> v(size);
        if(RAND_bytes(&v[0],size) != 1) {
            throw std::runtime_error("random bytes too predictable");
        }
        return v;
    }
   
    //try to be safe but if it isn't, generate a pseudo-random number of bytes
    std::vector<uint8_t> Random::bytes_safe_attempt(uint32_t size){
        //TODO
        std::vector<uint8_t> v(size);
        if(RAND_bytes(&v[0],size) != 1) {
            return v;
        }
        return v;
    }
    
    //generate pseudo-random bytes
    std::vector<uint8_t> Random::bytes_pseudo(uint32_t size)   {
        //TODO
        std::vector<uint8_t> v(size);
        return v;
    }
}

//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef UTILITIES_PRINT
#define UTILITIES_PRINT

#include <cstdint>
#include <iomanip>

namespace Utilities {
    class Print {
        public:
        static void hex(const void *begin, uint64_t size);
   
    };
}
#endif

//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef UTILITIES_DEBUGGING
#define UTILITIES_DEBUGGING

#include <cstdint>
#include <functional>
#include <string>
#include <iostream>

namespace Utilities {
    class Debugging {
        public:

        template<class T>
        static std::function<void(T *ptr)> deleter(const std::string &context) {
            return [&context](T *ptr){
                std::cout << context << std::endl;
                delete ptr;
            };
        }
        
        template<class T>
        static std::function<void(T *ptr)> deleter_arr(const std::string &context) {
            return [&context](T *ptr){
                std::cout << context << std::endl;
                delete[] ptr;
            };
        }
   
    };
}
#endif

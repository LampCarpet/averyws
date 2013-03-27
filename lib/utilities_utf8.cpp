//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_utf8.hpp>

#include <cstdint>
#include <iostream>
#include <iomanip>

namespace Utilities {
namespace Utf8 {

    bool Byte::valid() const{
        return it_ >= 0;
    }

    bool Byte::complete() const {
        return it_ == 0;
    }

    bool Byte::validate(const uint8_t byte) {
        std::cout << "Byte:" << std::hex << std::setfill('0') << std::setw(2) << (int) byte  << std::endl;
        if ( it_ == 0 ) {
            return set_length(byte);
        }
        
        if ( (byte & 0xC0) != 0x80){ 
            std::cout << "1" << " " << length_ <<std::endl;
            it_ = -1;
            return false;
        }
        
        if(length_ == 4 && it_ == 3 && previous_byte_ == 0xF4 && byte > 0x8F) {
            std::cout << "2" << length_ <<std::endl;
            it_ = -1;
            return false;
        } 
        
        if(length_ == 4 && it_ == 3 && previous_byte_ == 0xF0 && byte < 0x90) {
            std::cout << "3" << length_ <<std::endl;
            it_ = -1;
            return false;
        } 
        
        if(length_ == 3 && it_ == 2 && previous_byte_ == 0xED && byte > 0xA0 && byte < 0xB0) {
            std::cout << "4" << length_ <<std::endl;
            it_ = -1;
            return false;
        } 
        
        if(length_ == 3 && it_ == 2 && previous_byte_ == 0xED && byte == 0xA0) {
            previous_byte_ = byte;
        } 
        
        if(length_ == 3 && it_ == 1 && previous_byte_ == 0xA0 && byte == 0x80) {
            std::cout << "5" << length_ <<std::endl;
            it_ = -1;
            return false;
        } 
        
        if(length_ == 3 && it_ == 2 && previous_byte_ == 0xE0 && byte < 0x80) {
            std::cout << "6" << length_ <<std::endl;
            it_ = -1;
            return false;
        } 
        
        --it_;
        return true;
    }

    bool Byte::set_length(const uint8_t byte) {
        if( (byte & 0x80) == 0x00) { 
            length_ = 1; 
            it_ = 0;
            return true;
        }
        
        if( (byte & 0xC0) == 0x80) { 
            if( byte == 0xE0) {
                previous_byte_ = byte;
            }
            it_ = -1;
            return false;
        }
        
        if( (byte & 0xE0) == 0xC0) {
            if ( byte == 0xC1 || byte == 0xC0) {
                it_ = -1;
                return false;
            }
           
            length_ = 2; 
            it_ = 1;
            return true;
        }
        
        if( (byte & 0xF0) == 0xE0) {
            length_ = 3; 
            it_ = 2;
            previous_byte_ = byte;
            return true;
        }
        
        if( (byte & 0xF8) == 0xF0) {
            if (byte > 0xF4) {
                it_ = -1;
                return false;
            }
            length_ = 4; 
            it_ = 3;
            previous_byte_ = byte;
            return true;
        }

        it_ = -1;
        return false;
    }
}
}

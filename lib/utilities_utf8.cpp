//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_utf8.hpp>

#include <cstdint>
#include <iostream>
#include <iomanip>

//there are lots of magic numbers in here for a simple reason: this is a binary protocol. using #defines won't help
//This nice description hopefully will
//
//What will help is this table:
//
//#bits |min     |max      |#bytes |Byte 1      |Byte 2      |Byte 3      |Byte 4 
//------|--------|---------|-------|------------|------------|------------|---------
//  7   |U+0000  |U+007F   |   1   |0xxxxxxx    |            |            |         
//  11  |U+0080  |U+07FF   |   2   |110xxxxx    |10xxxxxx    |            |  
//  16  |U+0800  |U+FFFF   |   3   |1110xxxx    |10xxxxxx    |10xxxxxx    |               
//  21  |U+10000 |U+1FFFFF |   4   |11110xxx    |10xxxxxx    |10xxxxxx    |10xxxxxx
//
//  illegal utf8 sequences :
//  NOTICE the spaces in the binary. Those spaces separate protocol from data.
//
//  I.   Continuation bytes start with 0b10 so if the first 2 bits are not equal to 0b10 the continuation byte is invalid
//       What does this mean? if we mask the first 2 bits with 0xC0 (0b1100) and the result isnt 0x80 (0b1999) 
//       then it's invalid 
//       
//  II.  Overly long utf8 encoding is invalid. If you can rewrite the same encoding with less bytes
//       your utf8 is overly long.
//       What does this imply: if there are 5 leading 0s then the character is invalid. 
//       Encodings of 1 byte in size obviously do not follow this rule.
//
//  III. utf8 only encodes up to 10FFFF
//       What does this mean? If there's a character encoded in 4 bytes, 
//       the and the first byte > 0xF4 (0b11110 100) then it's invalid.
//       If it is equal to 0xF4 there still is a chance that the value contained is too big so we save 0xF4
//       and we wait for the 2nd byte. If the 2nd byte in the sequence is >= 0x90 (0b10 010000) it's invalid
//
//  IV.  The range 0xD800-0xDFFF is not valid
//       What does this mean? If there's a character encoded in 3 bytes and the first byte is 0xED (0b1110 1101)
//       there is a chance that the data is in that range so we save 0xED and we wait for the 2nd byte.
//       If the 2nd byte in the sequence is > 0xA0 (0b10 100000) and < 0xB0 (0b10 110000) it's invalid
//
//  V.   0xC1 and 0xC0 are never valid in utf8 because in no cases will they not produce error I. The reasons
//       are long to explain so take it for granted.

// hex  binary  | hex  binary
// --------------------------
// 0x0  0b0000  | 0x8  0b1000
// 0x1  0b0001  | 0x9  0b1001
// 0x2  0b0010  | 0xA  0b1010
// 0x3  0b0011  | 0xB  0b1011
// 0x4  0b0100  | 0xC  0b1100
// 0x5  0b0101  | 0xD  0b1101
// 0x6  0b0110  | 0xE  0b1110
// 0x7  0b0111  | 0xF  0b1111

namespace Utilities {
namespace Utf8 {

    bool Byte::valid() const{
        return it_ >= 0;
    }

    bool Byte::complete() const {
        return it_ == 0;
    }

    bool Byte::validate(const uint8_t byte) {
        if ( it_ == 0 ) {
            return set_length(byte);
        }
        
        //error I
        if ( (byte & 0xC0) != 0x80){ 
            std::cout << "@@1" << std::endl;
            it_ = -1;
            return false;
        }
       
       
        //error II: 0xF08F (0b11110 000 10 001111) has 5 zeros in a row
        //                          ^^^    ^^
        if(length_ == 4 && it_ == 3 && previous_byte_ == 0xF0 && byte <= 0x8F) {
            std::cout << "@@2" << std::endl;
            it_ = -1;
            return false;
        } 
       
        //error II: 0xE090 (0b1110 0000 10 011111) has 5 zeros in a row
        //                         ^^^^    ^
        if(length_ == 3 && it_ == 2 && previous_byte_ == 0xE0 && byte <= 0x9F) {
            std::cout << "@@3" << std::endl;
            it_ = -1;
            return false;
        } 

        //error III: 0xF490 (0b11110 100 10 010000) encodes the first 2 bytes to 11XXXX which means that it's over 10FFFF
        //                           ^^^    ^^ 
        //zero-filled, so the 0b10001  highlighted above is actually 0b00010001
        if(length_ == 4 && it_ == 3 && previous_byte_ == 0xF4 && byte >= 0x90) {
            std::cout << "@@4" << std::endl;
            it_ = -1;
            return false;
        } 
        
       
        //error IV: 0xED84 (0b1110 1101 10 100000) encodes D8 which is the start of the illegal sequence
        //                         ^^^^    ^^^^
        //it so happens that 3 bytes has no zero filling
        if(length_ == 3 && it_ == 2 && previous_byte_ == 0xED && byte >= 0xA0) {
            std::cout << "@@5" << std::endl;
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
        
        if( (byte & 0xE0) == 0xC0) {
            //error V (error II implied)
            if ( byte == 0xC1 || byte == 0xC0) {
                std::cout << "@@6" << std::endl;
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
            //error III
            if (byte > 0xF4) {
                std::cout << "@@7" << std::endl;
                it_ = -1;
                return false;
            }
            length_ = 4; 
            it_ = 3;
            previous_byte_ = byte;
            return true;
        }
        
        //error, it's a continuation byte (0b10 XXXXXX) but this function is for headers only 
        if( (byte & 0xC0) == 0x80) { 
            std::cout << "@@8" << std::endl;
            it_ = -1;
            return false;
        }

        std::cout << "@@9" << std::endl;
        it_ = -1;
        return false;
    }
}
}

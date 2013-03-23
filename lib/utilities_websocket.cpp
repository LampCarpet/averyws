//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <utilities_websocket.hpp>
#include <cstdint>
#include <utilities_random.hpp>
#include <utilities_utf8.hpp>
#include <utilities_print.hpp>
#include <iostream>

namespace Utilities {

uint64_t Websocket::reserve(const uint64_t length , const uint16_t flags) {
    uint64_t base_required = 2;
    
    uint8_t flags_8bit = (flags & 0x00ff) >> 8;
    if(flags_8bit & 0x80) base_required += 4;
    if(length > 125) {
        if(length > 65535) {
            base_required += 8;
        } else {
            base_required += 2;
        }
    }
    return base_required;
}

void Websocket::makeHeader(uint8_t *data, const uint64_t data_size, const uint64_t length,const uint16_t flags) {
    if(Websocket::reserve(length , flags) > data_size) {
        //todo throw exception
    }
    uint8_t header_size = 1;

    flags2header(data,flags);
    header_size += length2header(data,length);
    
    if( data[1] & 0x80) {
        generateMask(data + header_size);
        header_size += 4;
    }
}

void Websocket::generateMask(uint8_t *data) {
        std::vector<uint8_t> random_bytes = Utilities::Random::bytes_safe(4);
        //std::cout << "mask: " << random_bytes[0] << " " << random_bytes[1] << " " << random_bytes[2] << " " << random_bytes[3] << std::endl;
        data[0] = static_cast<uint8_t>(random_bytes[0]);
        data[1] = static_cast<uint8_t>(random_bytes[1]);
        data[2] = static_cast<uint8_t>(random_bytes[2]);
        data[3] = static_cast<uint8_t>(random_bytes[3]);
}

void Websocket::flags2header(uint8_t *data, uint16_t flags) {
    data[0]  = 0x00;
    data[1] &= 0x7F;

    uint8_t flags_8bit = (flags & 0xff00) >> 8;

    data[0] = 
        (flags_8bit & 0x80)
     |  (flags_8bit & 0x40)
     |  (flags_8bit & 0x20)
     |  (flags_8bit & 0x10)
     |  (flags_8bit & 0x00)
     |  (flags_8bit & 0x01)
     |  (flags_8bit & 0x02)
     |  (flags_8bit & 0x03)
     |  (flags_8bit & 0x09)
     |  (flags_8bit & 0x0A);
    
    flags_8bit = (flags & 0x00ff) >> 0;
    
    data[1] = (data[1] & 0x7F) | (flags_8bit & 0x80);

}

uint8_t Websocket::length2header(uint8_t *data, uint64_t length){
    if(length <= 125) {
        data[1] = length | (data[1] & 0x80); 
        return 1;
    } else if (length <= 65535) {
        data[1] = 126 | (data[1] & 0x80);
        data[2] = (length & 0x000000000000ff00) >>  8;
        data[3] = (length & 0x00000000000000ff) >>  0;
        return 3;
    } else {
        data[1] = 127 | (data[1] & 0x80);
        //read 8 bytes
        data[2] = (length & 0xff00000000000000) >> 56;
        data[3] = (length & 0x00ff000000000000) >> 48;
        data[4] = (length & 0x0000ff0000000000) >> 40;
        data[5] = (length & 0x000000ff00000000) >> 32;
        
        data[6] = (length & 0x00000000ff000000) >> 24;
        data[7] = (length & 0x0000000000ff0000) >> 16;
        data[8] = (length & 0x000000000000ff00) >>  8;
        data[9] = (length & 0x00000000000000ff) >>  0;

        return 9;
    }
}

void Websocket::applyMask(uint8_t* it,const uint64_t length,const uint8_t* mask,uint8_t &mask_offset,bool is_utf8,int8_t &utf8_offset) {
    if( mask[0] == 0x00 && mask[1] == 0x00 && mask[2] == 0x00 && mask[3] == 0x00) {
        //todo utf8 validation
        return ;
    }
    if( length == 0) { return ; }

    auto end = it + length;
    if(length >= 4) {
        for( auto end_4 = end-4 ; it <= end_4; it+=4) {
            //std::cout << "consumed 4 bytes " << *it << *(it+1)<< *(it+2)<< *(it+3);
            it[0] ^= mask[(mask_offset + 0) % 4];
            if(Utf8::validate(it[0],utf8_offset) < 0){return;}
            it[1] ^= mask[(mask_offset + 1) % 4];
            if(Utf8::validate(it[0],utf8_offset) < 0){return;}
            it[2] ^= mask[(mask_offset + 2) % 4];
            if(Utf8::validate(it[0],utf8_offset) < 0){return;}
            it[3] ^= mask[(mask_offset + 3) % 4];
            if(Utf8::validate(it[0],utf8_offset) < 0){return;}
            //std::cout << " to " << *it << *(it+1)<< *(it+2)<< *(it+3) << std::endl;
        }
    }
    //there can potentially be 4 bytes left in the data stream to be unmasked 1 byte at a time
    for(;it < end; ++it, ++mask_offset) {
            //std::cout << "consuming 1 byte " << *it;
            *it ^= mask[mask_offset % 4];
            if(Utf8::validate(*it,utf8_offset) < 0){return;}
            //std::cout << " to " << *it <<  std::endl;
    }
}

}

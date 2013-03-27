//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ws_control.hpp>
#include <ws_header.hpp>
#include <cstdint>
#include <array>
#include <memory>
#include <utilities_websocket.hpp>
#include <utilities_utf8.hpp>
#include <utilities_print.hpp>
#include <iostream>

namespace Websocket {
    void Control::new_buffer() {
        buffer_ = std::make_shared<std::array<uint8_t,129> >();
    }
    
    uint8_t* Control::payload_begin() {
        return &buffer_->at(2); 
    }
    
    uint8_t* Control::begin() {
        return &buffer_->at(0); 
    }
    
    uint8_t Control::size() const {
        return size_;
    }
    
    uint8_t Control::capacity() const {
        return buffer_->size();
    }
    
    ControlState Control::state() {
        return state_; 
    }

    int Control::process(Header &header) {
        size_ = header.payload_size() + 2;
        Utilities::Utf8::Byte utf8;
        uint8_t mask_offset = 0;
        uint8_t *buffer = &(*buffer_.get())[0];
        Utilities::Websocket::applyMask(&buffer[2]
                , header.payload_size()
                , &header.mask(), mask_offset
                , false
                , utf8);

        if(!utf8.complete()) {
            return 1007;
        }

        buffer[0] = header.begin()[0];
        buffer[1] = header.begin()[1];
        
        Utilities::Print::hex(&buffer[0],header.payload_size()+2); std::cout << std::endl;
       
    if(   (buffer[0] & 0x0F) == 0x08) {
        state_ = ControlState::KILL;
        std::cout << "control kill" << std::endl;
        buffer[1] &= 0x7F;
    } else if ( (buffer[0] & 0x0F) == 0x09 ) {
        state_ = ControlState::PING;
        buffer[0] = ( buffer[0] & 0xF0 ) | 0x0A;
        buffer[1] &= 0x7F;
        std::cout << "control pong"; Utilities::Print::hex(&buffer[0],2);std::cout <<std::endl;
    } else if ( (buffer[0] & 0x0F) == 0x0A ) {
        state_ = ControlState::PONG;
        std::cout << "pong recieved" << std::endl;
    } else {
      state_ = ControlState::INVALID;
    }
    return 0;
    }

}

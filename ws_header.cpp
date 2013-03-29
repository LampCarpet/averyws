//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ws_header.hpp>
#include <memory>
#include <array>
#include <iostream>
#include <utilities_print.hpp>

namespace Websocket {
    void Header::new_buffer() {
        buffer_ = std::make_shared<std::array<uint8_t,14> >();
        next_read_size_ = 2;
        state_ = HeaderState::FIRST;
    }
    
    bool Header::is_fin() const { return buffer_->at(0) & 0x80;}

    uint8_t* Header::begin() {
        return &buffer_->at(0);
    }

    const uint64_t Header::size() const {
        return buffer_->size();
    }

    int Header::process(bool new_request) {
        if ( state_ == HeaderState::FIRST) {
            return process_state_1(new_request);
        } else if ( state_ == HeaderState::SECOND) {
            return process_state_2(new_request);
        }
        return 1002; 
    }
      
    uint8_t* Header::current_position() {
        if(state_ == HeaderState::FIRST) {
            return &buffer_->at(0);
        } else if ( state_ == HeaderState::SECOND ) {
            return &buffer_->at(2);
        } else {
            return 0;
        }
    }

    uint64_t Header::size_left() {
        if(state_ == HeaderState::FIRST) {
            return buffer_->size();
        } else if ( state_ == HeaderState::SECOND ) {
            return buffer_->size() - 2;
        } else {
            return buffer_->size() - next_read_size_;
        }
    }

    uint64_t Header::next_read_size() const {
        return next_read_size_;
    }
    
    void Header::transfer(Header &header) {
        buffer_ =  header.buffer_;
        mask_ = header.mask_;
        payload_size_ = header.payload_size_;
        is_binary_ = header.is_binary_;
    }



    int Header::process_state_1(bool new_request){
        if (       ((buffer_->at(0) & 0x70) != rsvc_)
                || ((buffer_->at(0) & 0x0F) ) == 0x03
                || ((buffer_->at(0) & 0x0F) ) == 0x04
                || ((buffer_->at(0) & 0x0F) ) == 0x05
                || ((buffer_->at(0) & 0x0F) ) == 0x06
                || ((buffer_->at(0) & 0x0F) ) == 0x07
                || ((buffer_->at(0) & 0x0F) ) == 0x0B
                || ((buffer_->at(0) & 0x0F) ) == 0x0C
                || ((buffer_->at(0) & 0x0F) ) == 0x0D
                || ((buffer_->at(0) & 0x0F) ) == 0x0E
                || ((buffer_->at(0) & 0x0F) ) == 0x0F
           ) {
            return 1002;
        } else {
            is_binary_ = (buffer_->at(0) & 0x02) == 0x02;

            payload_size_ = buffer_->at(1) & 0x7F;
            mask_ = 0;


            next_read_size_ = 0;
            if(payload_size_ <= 125) {
                next_read_size_ = 0;
            } else if(payload_size_ == 126) {
                next_read_size_ = 2;
            } else if (payload_size_ == 127) {
                next_read_size_ = 8;
            }

            if( buffer_->at(1) & 0x80) {
                mask_ = &buffer_->at(2 + next_read_size_);
                next_read_size_ += 4;
            } else {
                mask_ = &buffer_->at(10);
                std::fill(mask_, mask_ + 4 ,0);
            }
            state_ = HeaderState::SECOND;
            return 0;
        }
    }
    
    int Header::process_state_2(bool new_request){
        if(payload_size_ == 126) {
            payload_size_ = (static_cast<uint64_t>(buffer_->at(2)) << 8) 
                | (static_cast<uint64_t>(buffer_->at(3)) << 0);
        } else if (payload_size_ == 127) {
            payload_size_ = (static_cast<uint64_t>(buffer_->at(2)) << 56) 
                | (static_cast<uint64_t>(buffer_->at(3)) << 48)
                | (static_cast<uint64_t>(buffer_->at(4)) << 40)
                | (static_cast<uint64_t>(buffer_->at(5)) << 32)
                | (static_cast<uint64_t>(buffer_->at(6)) << 24)
                | (static_cast<uint64_t>(buffer_->at(7)) << 16)
                | (static_cast<uint64_t>(buffer_->at(8)) <<  8) 
                | (static_cast<uint64_t>(buffer_->at(9)) <<  0);
        }

    if(   (buffer_->at(0) & 0x0F) == 0x08
       || (buffer_->at(0) & 0x0F) == 0x09 
       || (buffer_->at(0) & 0x0F) == 0x0A ) {
        
        if((buffer_->at(0) & 0x80) != 0x80) {
            return 1007;
        }

        if(payload_size_ > 125) {
            return 1002;
        }
        state_ = HeaderState::CONTROL;
    } else if(new_request && (buffer_->at(0) & 0x0F) == 0x00) {
                return 1002;
    } else if(!new_request && (buffer_->at(0) & 0x0F ) != 0x00 ) {
            return 1002;
    } else {
        state_ = HeaderState::DATA;
    }
    return 0;
}

}

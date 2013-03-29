//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ws_gather.hpp>
#include <ws_header.hpp>
#include <utilities_chunk_vector.hpp>
#include <utilities_websocket.hpp>
#include <iostream>

namespace Websocket {
    int Gather::read_chunk(Header &header,ChunkVector &buffer,uint64_t amount_consumed ,bool &new_request) {
          if(amount_consumed != 0) {
              Utilities::Websocket::applyMask(buffer_position_
                      , amount_consumed
                      , &header.mask(),mask_offset_
                      , !header.is_binary()
                      , utf8_);

              if(!utf8_.valid()) {
                  std::cout << "utf8 failure" << std::endl;
                  return 1007;
              }
          total_consumed_ += amount_consumed;
          }

          if(total_size_ == total_consumed_) {
              if(new_request == true) {
                  if(!utf8_.complete()) {
                      std::cout << "utf8 failure, not completed" << std::endl;
                      return 1007;
                  //} else if( (total_size_ == 0 && total_consumed_ == 0 && amount_consumed == 0)) {
                  //    buffer.close_last_chunk( 0 );
                  } else {
                      buffer.close_last_chunk( ( buffer.chunk_size() + total_size_ - 1) % buffer.chunk_size() + 1 );
                  }
              }
              next_consume_ = 0;
          } else {
              if (total_consumed_ == buffer.size()) {
                buffer.new_chunk();
              }
              buffer_position_ = &buffer.at(total_consumed_);

              if(total_size_ > buffer.size()) {
                next_consume_ = buffer.size() - total_consumed_;
              } else {
                next_consume_ = total_size_ - total_consumed_;
              }
          }

          //std::cout << "payload total size: " << total_size_ 
          //    << " payload total consumed: " << total_consumed_
          //    << " next consume amout: " << next_consume_
          //    << " total buffer size " << buffer.size() << std::endl;

          return 0;

    }
      
    void Gather::reset() {
       utf8_ = Utilities::Utf8::Byte();
       mask_offset_ = 0;

       total_size_ = 0;
       total_consumed_ = 0;
      
       next_consume_ = 0;

       buffer_position_ = 0;
    }
      
    void Gather::more(uint64_t size) {
        //std::cout << "payload increased by " << size << " from " << total_size_ << " to " << total_size_ + size << std::endl;
        total_size_ += size;
        mask_offset_ = 0;
    }
}

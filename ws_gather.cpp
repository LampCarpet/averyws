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
    int Gather::read_chunk(Header &header,ChunkVector &buffer,uint64_t amount_consumed ,bool &out_new_request) {

          if(amount_consumed != 0) {
              Utilities::Websocket::applyMask(buffer_position_
                      , amount_consumed
                      , &header.mask(),mask_offset_
                      , !header.is_binary()
                      , utf8_expected_);

              if(utf8_expected_ < 0) {
                  return 1007;
              }
          total_consumed_ += amount_consumed;
          last_consumed_ = amount_consumed;
          }

          if(total_size_ == total_consumed_) {
              out_new_request = header.is_fin();
              if(out_new_request == true) {
                  if(utf8_expected_ != 0) {
                      return 1007;
                  } else if(total_size_ == 0 && total_consumed_ == 0 && amount_consumed == 0) {
                      buffer.close_last_chunk( 0 );
                  } else {
                      buffer.close_last_chunk( ( ( total_size_ - 1 ) % buffer.chunk_size() ) + 1 );
                  }
              }
              next_consume_ = 0;
          } else {
              if(total_size_ >= buffer.size()) {
                  buffer.new_chunk();
              }

              if(total_size_ > buffer.size()) {
                  amount_consumed = buffer.size() - total_consumed_;
              }
              next_consume_ = amount_consumed;
          }

          std::cout << "payload total size: " << total_size_ 
              << " payload total consumed: " << total_consumed_
              << " payload last consumed: " << last_consumed_ 
              << " next consume amout: " << next_consume_
              << " total buffer size " << buffer.size() << std::endl;
          buffer_position_ = &buffer.at(total_consumed_);
          return 0;

    }
}

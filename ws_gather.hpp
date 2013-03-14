//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WEBSOCKET_GATHER
#define WEBSOCKET_GATHER

#include <cstdint>
#include <ws_header.hpp>
#include <utilities_chunk_vector.hpp>

namespace Websocket{

using Utilities::ChunkVector;

class Gather {
    public:
      int read_chunk(Header &header,ChunkVector &buffer,uint64_t inout_amount_consumed ,bool &out_new_request);
      uint64_t last_consumed() { return last_consumed_; }
      uint64_t next_consume() { return next_consume_; }
      uint64_t total_size() { return total_size_; }
      uint64_t total_consumed() { return total_consumed_; }
      uint8_t* current_position() { return buffer_position_; }
    private:
      int8_t utf8_expected_;
      uint8_t mask_offset_;

      uint64_t total_size_;
      uint64_t total_consumed_;
      
      uint64_t last_consumed_;
      uint64_t next_consume_;

      uint8_t *buffer_position_;
};

}
#endif

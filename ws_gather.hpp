//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WEBSOCKET_GATHER
#define WEBSOCKET_GATHER

#include <cstdint>
#include <ws_header.hpp>
#include <utilities_chunk_vector.hpp>
#include <utilities_utf8.hpp>

namespace Websocket{

using Utilities::ChunkVector;
//book-keeping class. This is the problem this class solves:
//We have 3 frames, one at 1600 byte, one at 250 bytes, and one at 1000 bytes
//and we have a scattered vector that has chunks of 1000 bytes.
//We want to fit the 3 frames as tightly as possible in the 1000 byte chunks. Some frames are too large
//to fit in one chunk and some frames are too small to fill a full chunk. Therefore in some cases we need
//to divide the frames and in other we need to concatenate them (and sometimes both!)
//This Gather class makes sure that data is read in the following way:
//1000 bytes (first chunk filled)
//500 bytes read (second chunk has 400 bytes of free space and the first frame has been read)
//frame 2 is fully read (second chunk now has only 150 bytes free)
//frame 3 is read in 2 parts, 150 bytes to fill the second chunk and then 750 bytes in a third chunk
//
//The final result is all 3 frames are read, which results in 3 chunks, 2 filled with 1000 bytes and 1 with 750 bytes for a total of 2850 bytes.
class Gather {
    public:
      int read_chunk(Header &header,ChunkVector &buffer, uint64_t inout_amount_consumed, bool &out_new_request);
      uint64_t next_consume() { return next_consume_; }
      uint64_t total_size() { return total_size_; }
      uint64_t total_consumed() { return total_consumed_; }
      uint8_t* current_position() { return buffer_position_; }
      void reset();
      void more(uint64_t size);
    private:
      Utilities::Utf8::Byte utf8_;
      uint8_t mask_offset_;

      uint64_t total_size_;
      uint64_t total_consumed_;
      
      uint64_t last_consumed_;
      uint64_t next_consume_;

      uint8_t *buffer_position_;
};

}
#endif

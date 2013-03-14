//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WS_HEADER
#define WS_HEADER

#include <cstdint>
#include <memory>
#include <array>

namespace Websocket {

enum class HeaderState {FIRST, SECOND, CONTROL, DATA, INVALID,FINISHED};

class Header{
  public:
    void new_buffer();
    uint8_t* begin();
    const uint64_t size() const;

    uint8_t* current_position();
    uint64_t size_left();
    uint64_t next_read_size() const;


    uint8_t& rsvc() { return rsvc_; }
    uint8_t rsvc() const { return rsvc_; }

    bool is_binary() const { return is_binary_; }
    bool is_fin() const; 
    HeaderState state() const { return state_; }
    const uint8_t& mask() const { return *mask_; }
    uint64_t payload_size() const { return payload_size_; }

    int process(bool new_request);
    void transfer(Header &header);

  private:
    int process_state_1(bool new_request);
    int process_state_2(bool new_request);

    HeaderState state_;

    uint8_t rsvc_;

    uint64_t next_read_size_;
    
    uint64_t payload_size_;
    uint8_t* mask_;
    std::shared_ptr<std::array<uint8_t,14> > buffer_;

    bool is_binary_;
};
}

#endif

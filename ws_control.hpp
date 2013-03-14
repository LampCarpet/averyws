//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WEBSOCKET_CONTROL
#define WEBSOCKET_CONTROL

#include <cstdint>
#include <array>
#include <memory>

namespace Websocket {

//forward declaration
class Header;

enum class ControlState {KILL,PING,PONG,INVALID};

class Control {
  public:
    void new_buffer();
    uint8_t* begin();
    uint64_t size() const;
    ControlState state();
    int process(Header &header);
    std::shared_ptr<std::array<uint8_t,129> > buffer() { return buffer_; }
    std::shared_ptr<std::array<uint8_t,129> > &&move_buffer() { return std::move(buffer_); }
  private:
    std::shared_ptr<std::array<uint8_t,129> > buffer_;
    ControlState state_;
};
}
#endif

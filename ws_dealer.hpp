//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WS_DEALER
#define WS_DEALER

#include <functional>
#include <memory>
#include <utilities_chunk_vector.hpp>

namespace Websocket {
    typedef std::function<void(std::shared_ptr<uint8_t>,uint64_t)> DealerCallback;
class Dealer {
  public:
    virtual void run(const DealerCallback &callback) = 0;
    virtual void send_open() = 0;
    virtual void send_close() = 0;
    virtual void send_last(Utilities::chunk_up &&chunk) = 0;
    virtual void send_more(Utilities::chunk_up &&chunk) = 0;
    virtual ~Dealer(){};
  private:
};
}

#endif

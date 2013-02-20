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

#ifndef ZMQ_DEALER
#define ZMQ_DEALER

#include <ws_dealer.hpp>
#include <zmq.hpp>
#include <functional>
#include <utilities_chunk_vector.hpp>

namespace Zmq {
class Dealer final : public Websocket::Dealer {
  public:
    Dealer(const std::string &address_outgoing,const std::string &address_incoming);
    virtual void run(const Websocket::DealerCallback &callback) override;
    virtual void send_open() override;
    virtual void send_close() override;
    virtual void send_last(Utilities::chunk_up &&chunk) override;
    virtual void send_more(Utilities::chunk_up &&chunk) override;

  private:
    const std::string address_incoming_;
    const std::string address_outgoing_;
    zmq::context_t context_;
    zmq::socket_t socket_pub_;
};
}

#endif

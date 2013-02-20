#include <zmq_dealer.hpp>

#include <ws_dealer.hpp>
#include <utilities_chunk_vector.hpp>
#include <zmq.hpp>
#include <memory>
#include <functional>
#include <string>

namespace {
//for zmq no copy
using namespace Utilities;
void freeUniqueChunk(void* data,void* hint) {
    delete reinterpret_cast<chunk_t *>(hint);

}

}

namespace Zmq {
using namespace Utilities;

Dealer::Dealer(const std::string &address_outgoing,const std::string &address_incoming):
    address_incoming_(address_incoming)
  , address_outgoing_(address_outgoing)
  , context_(1)
  , socket_pub_(context_,ZMQ_PUB) {
      socket_pub_.connect(address_outgoing.c_str());
  }


void Dealer::run(const Websocket::DealerCallback &callback) {
    zmq::socket_t socket (context_, ZMQ_SUB);
    socket.connect (address_outgoing_.c_str());
    socket.setsockopt(ZMQ_SUBSCRIBE,"",0);
    for (;;) {
             zmq::message_t *message = new zmq::message_t();
             socket.recv(message);
             //custom deleter will call the dtor of message which releases the data stored
             auto deleter = [message](uint8_t *ptr){delete message;};
             callback(std::shared_ptr<uint8_t>(static_cast<uint8_t *>(message->data()),deleter),message->size());
    }
}

void Dealer::send_open() {
    //no-op
}

void Dealer::send_close() {
    //no-op
}

void Dealer::send_last(chunk_up &&chunk) {
      //zmq is now in charge of managing the memory
      chunk_t *ptr = chunk.release();
      zmq::message_t zmq_request ( &ptr[0]
              ,ptr->size()
              ,freeUniqueChunk
              ,ptr);
      socket_pub_.send(zmq_request);
}

void Dealer::send_more(chunk_up &&chunk) {
      //zmq is now in charge of managing the memory
      chunk_t *ptr = chunk.release();
      zmq::message_t zmq_request ( &ptr[0]
              ,ptr->size()
              ,freeUniqueChunk
              ,ptr);
      socket_pub_.send(zmq_request,ZMQ_SNDMORE);
}

}

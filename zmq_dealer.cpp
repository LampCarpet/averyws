//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <zmq_dealer.hpp>

#include <ws_dealer.hpp>
#include <utilities_chunk_vector.hpp>
#include <zmq.hpp>
#include <memory>
#include <functional>
#include <string>

#include <iostream>

namespace {
//for zmq no copy
using namespace Utilities;
void freeUniqueChunk(void* data,void* hint) {
    //std::cout << "freeing data" << std::endl;
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
    socket.connect (address_incoming_.c_str());
    socket.setsockopt(ZMQ_SUBSCRIBE,"B",1);
    socket.setsockopt(ZMQ_SUBSCRIBE,"U",1);
    for (;;) {
             zmq::message_t type;
             socket.recv(&type);

             zmq::message_t *message = new zmq::message_t();
             socket.recv(message);
             //custom deleter will call the dtor of message which releases the data stored
             //std::cout << "zmq recieved " << message->size() << " "  << std::string(static_cast<char *>(message->data()),message->size()) << std::endl;
             std::cout << "zmq recieved " << message->size() << std::endl;
             auto deleter = [message](uint8_t *ptr){delete message;};
             callback(std::shared_ptr<uint8_t>(static_cast<uint8_t *>(message->data()),deleter),message->size(),static_cast<uint8_t *>(type.data())[0] == 'B');
    }
}

void Dealer::send_open(bool is_binary) {
      static char B = 'B';
      static char U = 'U';
      if(is_binary) {
        zmq::message_t zmq_request ( &B ,1 ,0 ,0);
        socket_pub_.send(zmq_request,ZMQ_SNDMORE);
      } else {
        zmq::message_t zmq_request ( &U ,1 ,0 ,0);
        socket_pub_.send(zmq_request,ZMQ_SNDMORE);
      }
}

void Dealer::send_close() {
    //no-op
}

void Dealer::send_last(chunk_up &&chunk) {
      //zmq is now in charge of managing the memory
      chunk_t *ptr = chunk.release();

      if(ptr->size() == 0) {
          std::cout << "empty" << std::endl;
          zmq::message_t zmq_request ( 0
                  ,0
                  ,freeUniqueChunk
                  ,ptr);
          socket_pub_.send(zmq_request);
      } else {
          //std::cout << "zmq sending last " << std::string(reinterpret_cast<char *>(&(ptr->at(0))),ptr->size()) << std::endl;
          zmq::message_t zmq_request ( &(ptr->at(0))
                  ,ptr->size()
                  ,freeUniqueChunk
                  ,ptr);
          socket_pub_.send(zmq_request);
      }
}

void Dealer::send_more(chunk_up &&chunk) {
      //zmq is now in charge of managing the memory
      chunk_t *ptr = chunk.release();
      //std::cout << "zmq sending more " << std::string(reinterpret_cast<char *>(&(ptr->at(0))),ptr->size()) << std::endl;
      zmq::message_t zmq_request ( &(ptr->at(0))
              ,ptr->size()
              ,freeUniqueChunk
              ,ptr);
      socket_pub_.send(zmq_request,ZMQ_SNDMORE);
}

}

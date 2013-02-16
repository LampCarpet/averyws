#include <ws_server.hpp>
#include <ws_session.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <cstdlib>
#include <thread>

#include <ws_session_manager.hpp>
#include <zmq.h>

#include <iostream>


namespace Websocket{

using namespace boost;
using namespace boost::asio;

Server::Server(
      io_service &service,
      const uint8_t num_threads,
      const std::string &address,
      const uint16_t port)
    : 
        num_threads_(num_threads)
      , response_strand_(service)
      , acceptor_(service,ip::tcp::endpoint(ip::tcp::v4(),port))
      , context_(1)
      , socket_pub_(context_,ZMQ_PUB)
    {

    socket_pub_.connect("tcp://localhost:5555");

    //todo: try if threads bail prematurely 
    //io_service::work work(service);
    start_accept();
  }


void Server::start_accept() {
    auto session = std::shared_ptr<Session>(new Session(acceptor_.get_io_service(),session_manager_ ,socket_pub_));
    
    acceptor_.async_accept(session->socket()
        ,std::bind(&Server::handle_accept,this,session,std::placeholders::_1));
}

void Server::handle_accept(std::shared_ptr<Session> session, const system::error_code& error) {
  if (!error) {
    std::cout << "handle_accept" << std::endl;
    session->start();
  }
  start_accept();
}

void Server::start() {
    std::set<std::unique_ptr<std::thread> > threads;

    for( unsigned int i = 0; i < num_threads_; ++i ) {
        threads.insert(std::unique_ptr<std::thread>(new std::thread( [&]() {
            acceptor_.get_io_service().run();
            //post strand-wrapped write
            }) 
        ));
    }

    threads.insert(std::unique_ptr<std::thread>(new std::thread( [&]() {
        zmq::socket_t socket (context_, ZMQ_SUB);
        socket.connect ("tcp://localhost:5555");
        socket.setsockopt(ZMQ_SUBSCRIBE,"",0);
        for (;;) {
                 auto shared = std::make_shared<zmq::message_t>();
                 socket.recv(shared.get());
        }
        })
    ));

    std::cout << threads.size() << " threads started" << std::endl;
    
    for(auto &t:threads) {
        t->join();
    }
    std::cout << threads.size() << " threads joined" << std::endl;
}

}

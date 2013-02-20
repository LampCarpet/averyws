#include <ws_server.hpp>
#include <ws_session.hpp>
#include <ws_dealer.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <cstdlib>
#include <thread>

#include <ws_session_manager.hpp>

#include <iostream>


namespace Websocket{

using namespace boost;
using namespace boost::asio;

Server::Server(
        io_service &service
      , const uint8_t num_threads
      , const uint16_t port
      , Dealer &dealer)
    : 
        num_threads_(num_threads)
      , acceptor_(service,ip::tcp::endpoint(ip::tcp::v4(),port))
      , dealer_(dealer)
      , io_strand_(acceptor_.get_io_service())
    {
    //todo: try if threads bail prematurely 
    //io_service::work work(service);
    start_accept();
  }


void Server::start_accept() {
    auto session = std::shared_ptr<Session>(new Session(acceptor_.get_io_service(),session_manager_ ,dealer_,io_strand_));
    
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
            }) 
        ));
    }
    
    threads.insert(std::unique_ptr<std::thread>(new std::thread( 
       [&]() {
           dealer_.run([&](std::shared_ptr<uint8_t> data, uint64_t size) {
           for(auto session : session_manager_.unauthed()) {
               std::cout << "unauthed output" << std::endl;
               acceptor_.get_io_service().post(io_strand_.wrap(std::bind(&Session::write,session,data,size)));
               std::cout << "'" << std::string(reinterpret_cast<char *>(data.get()),size) << "'" <<std::endl;    
           }
           });
       })
    ));

    std::cout << threads.size() << " threads started" << std::endl;
    
    for(auto &t:threads) {
        t->join();
    }
    std::cout << threads.size() << " threads joined" << std::endl;
}

}

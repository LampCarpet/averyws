//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <iostream>
#include <ws_server.hpp>
#include <zmq_dealer.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <set>

int main(int argc, char* argv[]) {
  using namespace Websocket;
  using namespace boost;
  using namespace boost::asio;

  if (argc != 5) {
    std::cerr << "Usage: ws_server <threads> <port websocket> <port incoming zmq> <port incoming zmq>\n";
    return 1;
  }

  int num_threads = std::atoi(argv[1]);
  int port = std::atoi(argv[2]);
  std::string zmq_address_in = "tcp://localhost:" + std::string(argv[3]); 
  std::string zmq_address_out = "tcp://localhost:" + std::string(argv[4]); 

  try {
    io_service service;
    Zmq::Dealer zmq_dealer(zmq_address_out, zmq_address_in);
    auto server = Server(service,num_threads,port,zmq_dealer);
    server.start();
    
    std::cout << "main:out of scope" << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << "\n";
  }

  return 0;
}

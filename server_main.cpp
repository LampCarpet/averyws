#include <iostream>
#include <ws_server.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <set>

int main(int argc, char* argv[]) {
  using namespace Websocket;
  using namespace boost;
  using namespace boost::asio;

  if (argc != 3) {
    std::cerr << "Usage: ws_server <threads> <port>\n";
    return 1;
  }

  int num_threads = std::atoi(argv[1]);
  int port = std::atoi(argv[2]);

  try {

    io_service service;
    auto server = Server(service,num_threads, "0.0.0.0",port);
    server.start();
    
    std::cout << "main:out of scope" << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << "\n";
  }

  return 0;
}

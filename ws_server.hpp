#ifndef WS_SERVER
#define WS_SERVER

#include <ws_session.hpp>
#include <boost/asio.hpp>
#include <system_error>
#include <memory>
#include <cstdlib>
#include <thread>

#include <ws_session_manager.hpp>

#include <zmq.hpp>

namespace Websocket {

using namespace boost::asio;
using namespace boost;

class Server {
public:
  Server( io_service &service, const uint8_t num_threads, const std::string &address, const uint16_t port);

  void start();
  void start_accept();
  void handle_accept(std::shared_ptr<Session> session, const system::error_code& error);
  void write(std::shared_ptr<zmq::message_t > message);

private:
  uint8_t num_threads_;
  io_service::strand response_strand_;
  ip::tcp::acceptor acceptor_;
  SessionManager session_manager_; 
  zmq::context_t context_;
  zmq::socket_t socket_pub_;
};

}
#endif

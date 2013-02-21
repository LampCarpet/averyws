//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WS_SERVER
#define WS_SERVER

#include <ws_session.hpp>
#include <boost/asio.hpp>
#include <system_error>
#include <memory>
#include <cstdlib>
#include <thread>

#include <ws_session_manager.hpp>


namespace Websocket {

using namespace boost::asio;
using namespace boost;

class Server {
public:
  Server( io_service &service, const uint8_t num_threads, const uint16_t port, Dealer &dealer);

  void start();
  void start_accept();
  void handle_accept(std::shared_ptr<Session> session, const system::error_code& error);

private:
  uint8_t num_threads_;
  ip::tcp::acceptor acceptor_;
  SessionManager session_manager_; 
  Dealer &dealer_;
  strand io_strand_;
};

}
#endif

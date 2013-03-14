//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef WS_SESSION
#define WS_SESSION

#include <boost/asio.hpp>
#include <set>
#include <array>
#include <cstdlib>
#include <utilities_chunk_vector.hpp>
#include <ws_control.hpp>
#include <ws_gather.hpp>
#include <ws_header.hpp>
#include <system_error>
#include <memory>

namespace Websocket {

class SessionManager;
class Dealer;

using namespace boost;
using namespace boost::asio;
using namespace Utilities;

class Session : public std::enable_shared_from_this<Session> {

public:
  Session(io_service& io_service
        , SessionManager& session_manager
        , Dealer &dealer
        , strand &io_strand
        );

  ip::tcp::socket& socket();

  void start();

  void handle_first_handshake(const system::error_code& error, size_t bytes_transferred);
  void read_first_header(const system::error_code& error, size_t bytes_transferred);
  void read_chunk(uint64_t previous_read, const system::error_code& error);
  void read_header();
  void handle_read_header(const system::error_code& error);
  void handle_read(const system::error_code& error);
  void handle_control_read(const system::error_code& error);
  void cancel_socket(uint16_t code);

  void request(ChunkVector_sp request);
  void write(std::shared_ptr<uint8_t> data, uint64_t size, bool is_binary);

  bool authenticated() const {return authenticated_;}
  const std::string & sid() const {return sid_;}
  ~Session(){ std::cout << "session destroyed" << std::endl;}
  

private:
  ip::tcp::socket socket_;
  strand strand_;
  io_service& io_service_;

  ChunkVector_sp buffer_;


  uint8_t rsvc_;

  bool new_request_;
  uint64_t first_handshake_size_;

  Header header_;
  Header temp_header_;
  Gather gather_;
  Control control_;
  

  SessionManager& session_manager_;
  std::string sid_;
  bool authenticated_;

  Dealer &dealer_;
  strand &io_strand_;
};

}
#endif

#ifndef WS_SESSION
#define WS_SESSION

#include <boost/asio.hpp>
#include <set>
#include <array>
#include <cstdlib>
#include <utilities_chunk_vector.hpp>
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
  void read_header();
  void handle_read_header_1(const system::error_code& error);
  void handle_read_header_2(const system::error_code& error);
  void handle_read(const system::error_code& error);
  void handle_control_read(const system::error_code& error);

  void request(ChunkVector_sp request);
  void write(std::shared_ptr<uint8_t> data, uint64_t size);

  bool authenticated() const {return authenticated_;}
  const std::string & sid() const {return sid_;}
  ~Session(){ std::cout << "session destroyed" << std::endl;}

private:
  ip::tcp::socket socket_;
  strand strand_;
  io_service& io_service_;
  SessionManager& session_manager_;

  ChunkVector_sp buffer_;

  std::shared_ptr<std::array<uint8_t,127> > control_buffer_;

  bool new_request_;
  uint64_t first_handshake_size_;
  uint64_t payload_read_;
  uint64_t payload_size_;
  uint8_t* mask_;
  std::shared_ptr<std::array<uint8_t,14> > header_buffer_;
        
  uint64_t temp_payload_read_;
  uint64_t temp_payload_size_;
  uint8_t* temp_mask_;
  std::shared_ptr<std::array<uint8_t,14> > temp_header_buffer_;
  std::string sid_;
  bool authenticated_;

  Dealer &dealer_;
  strand &io_strand_;
};

}
#endif

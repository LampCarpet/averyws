//          Matthew Avery Coder 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ws_session.hpp>

#include <boost/asio.hpp>

#include <utilities_websocket.hpp>
#include <utilities_chunk_vector.hpp>
#include <ws_session_manager.hpp>

#include <iomanip>

#include <system_error>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <memory>

#include <array>
#include <string>


#include <openssl/sha.h>
#include <base64.hpp>

#include <ws_dealer.hpp>

#include <cstring>

#include <utilities_print.hpp>


namespace Websocket {

using namespace boost;
using namespace boost::asio;
using namespace Utilities;

  Session::Session(io_service& io_service
          , SessionManager &session_manager
          , Dealer &dealer
          , strand &io_strand
          )
  :  socket_(io_service)
   , strand_(io_service)
   , io_service_(io_service)
   , session_manager_(session_manager)
   , authenticated_(false)
   , dealer_(dealer)
   , io_strand_(io_strand){
  }

  ip::tcp::socket& Session::socket() {
    return socket_;
  }

  void Session::start() {
    session_manager_.add_unauthed(shared_from_this());
    buffer_ = ChunkVector_sp ( new ChunkVector());
    buffer_->new_chunk();

    new_request_ = true;
    socket_.async_read_some(
              buffer(&buffer_->at(0), buffer_->size())
            , strand_.wrap(std::bind(&Session::read_first_header,shared_from_this(),std::placeholders::_1,std::placeholders::_2)));
  }

  void Session::read_first_header(const system::error_code& error, size_t bytes_transferred) {
      if (error) { session_manager_.remove(shared_from_this()); return;}
      static const char magic_number[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
      static const char search[] = "\r\nSec-WebSocket-Key: ";
      static const char response[] = "HTTP/1.1 101 Switching Protocols\r\n"
                                     "Upgrade: websocket\r\n"
                                     "Connection: Upgrade\r\n"
                                     "Sec-WebSocket-Accept: ";
      static const char response_end[] = "\r\n\r\n";

      //null terminate
      buffer_->at(bytes_transferred-1) = '\0';
      first_handshake_size_ = bytes_transferred;

      auto found = std::strstr(reinterpret_cast<char *>(&buffer_->at(0)),search);
      found += (sizeof(search) -1);
      if(found == 0){
          std::cout << "could not find key" << std::endl;
          session_manager_.remove(shared_from_this());
          return;
      }

      auto found_end = std::strstr(found+sizeof(search),"\r\n");
      
      if(found_end == 0){
          std::cout << "could not find end of key" << std::endl;
          session_manager_.remove(shared_from_this());
          return;
      }

      //magic number is null terminated so no need to add an extra character
      uint64_t size = sizeof(magic_number) + (found_end - found);
      std::unique_ptr<uint8_t[]> input(new uint8_t[size]);

      std::copy(found,found_end,input.get());
      //magic number is null terminated so no need to null terminate
      std::copy(magic_number,magic_number + sizeof(magic_number)-1,input.get() + (found_end - found));
      uint8_t obuf[20];
      SHA1(input.get(),size-1,obuf);

      //Utilities::Print::hex(obuf,sizeof(obuf)); std::cout << std::endl;
      //todo this is fixed size, use char[]
      std::string b64 = base64_encode(obuf,20);

      std::copy(response,response + (sizeof(response)-1),&buffer_->at(0));
      std::copy(b64.c_str(),b64.c_str() + b64.size(),&buffer_->at(sizeof(response)-1));
      std::copy(response_end,response_end + (sizeof(response_end)-1),&buffer_->at(sizeof(response)-1+b64.size()));

      //TODO zlib negociation??
      temp_header_.rsvc() = 0x00;

      auto this_shared = shared_from_this();
      async_write(socket_
              , buffer(&buffer_->at(0), sizeof(response)-1+b64.size()+sizeof(response_end)-1)
              , strand_.wrap([this,this_shared](const system::error_code& error, size_t bytes_transferred) {
                  if (error) { 
                    std::cout << "error" << std::endl;
                    session_manager_.remove(this_shared); 
                    return;
                  }
                  read_header();
              }
              ));
  }

  void Session::read_header() {
    //std::cout << "in read header" << std::endl;
    temp_header_.new_buffer();
    async_read(socket_ ,
            buffer(temp_header_.current_position(), temp_header_.size_left()) ,
            transfer_exactly(temp_header_.next_read_size()) ,
            strand_.wrap(std::bind(&Session::handle_read_header, shared_from_this(), std::placeholders::_1)));
  }

  void Session::handle_read_header(const system::error_code& error) {
    if (!error) {
        int rcode = temp_header_.process(new_request_);
        if(rcode != 0) {
            cancel_socket( rcode );
            return;
        }
        if(temp_header_.is_fin()) {
            if(temp_header_.state() == HeaderState::CONTROL) {
                control_.new_buffer();
                async_read(socket_ ,
                        buffer(control_.begin(),control_.size()) , 
                        transfer_exactly(temp_header_.payload_size()),
                        strand_.wrap(std::bind(&Session::handle_control_read, shared_from_this(), std::placeholders::_1)));
                return;
            } else if (temp_header_.state() == HeaderState::DATA) {
                header_.transfer(temp_header_);
                if(new_request_) {
                    buffer_ = ChunkVector_sp( new ChunkVector());
                }
                read_chunk(0,system::error_code());
            }
        } else {
            async_read(socket_ ,
                    buffer(temp_header_.current_position(), temp_header_.size_left()) ,
                    transfer_exactly(temp_header_.next_read_size()) ,
                    strand_.wrap(std::bind(&Session::handle_read_header, shared_from_this(), std::placeholders::_1)));
        }
    } else {
      session_manager_.remove(shared_from_this());
    }
    
  }

  void Session::read_chunk(uint64_t previous_read, const system::error_code& error) {
      if (!error) {
          int rcode = gather_.read_chunk(header_,*buffer_.get(),previous_read,new_request_);
          if(rcode != 0) {
            cancel_socket( rcode );
          } else if (new_request_ == true) {
              io_service_.post(io_strand_.wrap(std::bind(&Session::request, shared_from_this(),buffer_)));
              read_header();
          } else {
              async_read(socket_
                      , buffer(gather_.current_position(),gather_.next_consume())
                      , transfer_exactly(gather_.next_consume())
                      , strand_.wrap(std::bind(&Session::read_chunk, shared_from_this(), gather_.next_consume(),  std::placeholders::_1)));
          }
      } else {
          session_manager_.remove(shared_from_this());
      }
  }
  
  void Session::request(std::shared_ptr<Utilities::ChunkVector > request) {
      dealer_.send_open( header_.is_binary() );
      if(request->size() == 0) {
          chunk_up chunk(new chunk_t());
          std::cout << "empty request " << std::endl;
          dealer_.send_last(std::move(chunk));
      } else {
          for(auto chunk = request->chunk_begin(); chunk != request->chunk_cend(); ++chunk) {
              //std::cout << "processing chunk" << std::endl;
              //std::cout << std::string(reinterpret_cast<char*>(&(*chunk)->at(0)),(*chunk)->size()) << std::endl;
              if(chunk+1 == request->chunk_end()) {
                  dealer_.send_last(std::move(*chunk));
              } else {
                  dealer_.send_more(std::move(*chunk));
              }

          }
      }
      dealer_.send_close();
  }

void Session::write(std::shared_ptr<uint8_t> data, uint64_t size,bool is_binary) {
    uint16_t flags = 0x8000;
    if(is_binary) {
        flags |= 0x0200;
    } else {
        flags |= 0x0100;
    } 
    uint64_t header_size = Utilities::Websocket::reserve(size,flags);
    std::shared_ptr<uint8_t> header(new uint8_t[header_size]);


    Utilities::Websocket::makeHeader(header.get(),header_size,size,flags);

    std::cout << "write header for size " << size << " "; Print::hex(header.get(),header_size);std::cout << std::endl;

    //double braces until the compiler supports single braces:
    //http://gcc.gnu.org/bugzilla/show_bug.cgi?id=25137

    std::array<const_buffer,2> buffers = {{
        buffer(header.get(),header_size),
        buffer(data.get(),size) }};
   
    auto this_shared = shared_from_this();
             
    //std::cout << "sending:" << std::string(reinterpret_cast<char *>(header.get()),header_size) << std::endl;
    //for(uint64_t i = 0; i < header_size; ++i) std::cout << std::hex << static_cast<unsigned int>(header.get()[i]) << " ";
    //std::cout << std::endl;
    //std::cout << "sending:" << std::string(reinterpret_cast<char *>(data.get()),size) << " " << size << std::endl;

    //std::cout << "header use count: " << header.use_count() << " " << data.use_count() << std::endl;

    async_write(socket_
          , buffers 
          , strand_.wrap([this,this_shared,data,header](const system::error_code& error,size_t bytes_transferred){
                //std::cout << "async header use count: " << header.use_count() << " " << data.use_count() << std::endl;
                if (error) {
                  std::cout << "write error" << std::endl;
                  session_manager_.remove(this_shared);
                }
          }));
}
    
void Session::handle_control_read(const system::error_code& error) {
  if (!error) {
      control_.process(temp_header_);
      if(control_.state() == ControlState::KILL) {
        auto this_shared = shared_from_this();
        auto control_buffer = control_.move_buffer();
        async_write(socket_
              , buffer(control_.begin(),control_.size())
              , strand_.wrap([this,this_shared,control_buffer](const system::error_code& error,size_t bytes_transferred){
                      socket_.cancel();
                      session_manager_.remove(this_shared);
              }));
      } else if( control_.state() == ControlState::PING) {
        read_header();
      } else if( control_.state() == ControlState::PONG) {
        auto this_shared = shared_from_this();
        auto control_buffer = control_.move_buffer();
        async_write(socket_
              , buffer(control_.begin(),control_.size())
              , strand_.wrap([this,this_shared,control_buffer](const system::error_code& error,size_t bytes_transferred){
                    if (error) {
                      std::cout << "write error" << std::endl;
                      session_manager_.remove(this_shared);
                    }
              }));
        read_header();
      } else if (control_.state() == ControlState::INVALID) {
        session_manager_.remove(shared_from_this());
      } else {
        //todo
        std::cout << "control state undefined" << std::endl;
      }
  } else {
    session_manager_.remove(shared_from_this());
  }
}

void Session::cancel_socket(uint16_t code) {
        auto header = std::make_shared<std::array<uint8_t,14> >();
        header->at(0) = 0x88 | rsvc_;
        header->at(1) = 0x00;
        header->at(2) = (code & 0xff00) >> 8;
        header->at(3) = (code & 0x00ff) >> 0;

        std::cout << "cancelling with "; Print::hex(&header->at(0),4); std::cout << std::endl;

        auto this_shared = shared_from_this();

        async_write(socket_
              , buffer(&header->at(0),4)
              , strand_.wrap([this,this_shared,header](const system::error_code& error,size_t bytes_transferred){
                      socket_.cancel();
                      session_manager_.remove(this_shared);
              }));
}

}

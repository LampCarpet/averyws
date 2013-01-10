//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>
#include <openssl/sha.h>
#include <base64.h>

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service),
      first_handshake(true)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    if(first_handshake) {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_first_handshake, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    } else {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  void handle_first_handshake(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::cout << "first handshake\n" << data_;
      std::string magic_number = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
      std::string search = "\r\nSec-WebSocket-Key: ";
      std::string data(data_);
      size_t found = data.find(search);

      if(found == std::string::npos){
          std::cout << "shit" << std::endl;
      }

      size_t found_end = data.find("\r\n",found+search.size());
      std::string input =  data.substr(found+search.size(),found_end-found-search.size()) + magic_number;
      unsigned char obuf[20];
      SHA1(reinterpret_cast<const unsigned char *>(input.c_str()),input.size(),obuf);
      for (int i = 0; i < 20; i++) {
          std::cout << std::hex << static_cast<unsigned int>(obuf[i]) << " ";
      }
      std::cout << std::endl;

      std::ostringstream oss;
      oss << "HTTP/1.1 101 Switching Protocols\r\n"
          << "Upgrade: websocket\r\n"
          << "Connection: Upgrade\r\n"
          << "Sec-WebSocket-Accept: " << base64_encode(obuf,20)
          << "\r\n\r\n";
      std::string temp = oss.str(); 

      std::copy(temp.begin(),temp.end(),data_back_);
      data_back_[temp.size()] = '\0';

      std::cout << data_back_ << std::endl;

      boost::asio::async_write(socket_,
          boost::asio::buffer(data_back_, oss.str().size()),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::cout << "in handle_read" << std::endl;
      for (int i = 0; i < bytes_transferred; i++) {
          std::cout << std::hex << static_cast<unsigned int>(*reinterpret_cast<unsigned char *>(&data_[i])) << " ";
      }
      std::cout << std::endl;
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      //boost::asio::async_write(socket_,
      //    boost::asio::buffer(data_, bytes_transferred),
      //    boost::bind(&session::handle_write, this,
      //      boost::asio::placeholders::error));
      std::cout << "out of handle_read" << std::endl;
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout << "in handle_write" << std::endl;
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      std::cout << "out of handle_write" << std::endl;
    }
    else
    {
      delete this;
    }
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char data_back_[max_length];
  bool first_handshake;
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
    }
    else
    {
      delete new_session;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

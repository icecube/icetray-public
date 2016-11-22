#ifndef HTTP_SOURCE_HPP
#define HTTP_SOURCE_HPP

#include <boost/asio.hpp>

struct http_source{
  typedef char char_type;

  struct category
    : boost::iostreams::source_tag,
      boost::iostreams::closable_tag { };

  http_source(const std::string &url);

  std::streamsize read(char *s, std::streamsize size);

  void close(){ socket_->close(); };

  boost::shared_ptr<boost::asio::io_service> io_service_;
  boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
  boost::shared_ptr<boost::asio::streambuf> buffer_;
};

#endif // HTTP_SOURCE_HPP

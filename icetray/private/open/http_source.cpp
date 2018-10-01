/**
 *  $Id: open.cxx 2595 2016-06-01 18:26:57Z cweaver $
 *
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <boost/asio.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string.hpp>
#include <archive/iterators/base64_from_binary.hpp>
#include <archive/iterators/transform_width.hpp>

#include <icetray/I3Logging.h>

#include "http_source.hpp"
using boost::asio::ip::tcp;
using boost::algorithm::ends_with;

// http://stackoverflow.com/a/16775827
const std::string base64_padding[] = {"", "==","="};
std::string base64_encode(std::string::const_iterator begin, std::string::const_iterator &end) {
  namespace bai = icecube::archive::iterators;

  std::stringstream os;

  // convert binary values to base64 characters
  typedef bai::base64_from_binary
  // retrieve 6 bit integers from a sequence of 8 bit bytes
  <bai::transform_width<const char *, 6, 8> > base64_enc; // compose all the above operations in to a new iterator

  std::copy(base64_enc(&*begin), base64_enc(&*end),
            std::ostream_iterator<char>(os));

  os << base64_padding[(end-begin) % 3];
  return os.str();
}

http_source::http_source(const std::string &url) :
  io_service_(new boost::asio::io_service),
  socket_(new tcp::socket(*io_service_)),
  buffer_(new boost::asio::streambuf)
{
  // parse the parts of the URL (based on http://stackoverflow.com/a/2616217)
  std::pair<std::string::const_iterator, std::string::const_iterator>
    protocol, auth, host, path, query;

  query.first = std::find(url.begin(), url.end(), '?');
  path.second = query.first;
  
  path.second = query.first;
  if (query.first != url.end())
    query.first++;
  
  protocol.first = url.begin();
  protocol.second = url.begin() + url.find("://");
  host.first = (protocol.second == url.end()) ? url.begin() : protocol.second + 3;
  host.second = std::find(host.first, query.first, '/');
  
  auth.second = std::find(host.first, host.second, '@');
  auth.first = auth.second;
  if (auth.second != host.second) {
    auth.first = host.first;
    host.first = auth.second+1;
  }
  path.first = host.second;
  
  // following boost/libs/asio/example/http/client/sync_client.cpp
  
  // Get a list of endpoints corresponding to the server name.
  tcp::resolver resolver(*io_service_);
  tcp::resolver::query dns_query(std::string(host.first, host.second),
				 std::string(protocol.first, protocol.second));
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(dns_query);
  tcp::resolver::iterator end;
  
  // Try each endpoint until we successfully establish a connection.
  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end){
    socket_->close();
    socket_->connect(*endpoint_iterator++, error);
  }
  
  if (error)
    throw boost::system::system_error(error);
  
  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  boost::asio::streambuf request;
  std::ostream request_stream(&request);
  request_stream << "GET " << std::string(path.first, path.second) << " HTTP/1.0\r\n";
  request_stream << "Host: " << std::string(host.first, host.second)  << "\r\n";
  request_stream << "Accept: */*\r\n";
  if (auth.first != auth.second) {
    request_stream << "Authorization: Basic " << base64_encode(auth.first, auth.second) << "\r\n";
  }
  request_stream << "Connection: close\r\n\r\n";
  
  // Send the request.
  boost::asio::write(*socket_, request);
  
  // Read the response status line. The response streambuf will automatically
  // grow to accommodate the entire line. The growth may be limited by passing
  // a maximum size to the streambuf constructor.
  boost::asio::read_until(*socket_, *buffer_, "\r\n");
  
  // Check that response is OK.
  std::istream response_stream(buffer_.get());
  std::string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  std::string status_message;
  std::getline(response_stream, status_message);
  if (!response_stream || http_version.substr(0, 5) != "HTTP/"){
    log_error("Invalid response");
    socket_->close();
    return;
  }
  
  if (status_code != 200){
    log_error_stream("Server responded with status code " << status_code);
    socket_->close();
    return;
  }
  
  // Read the response headers, which are terminated by a blank line.
  // NB: read_until() may read an arbitrary number of bytes
  boost::asio::read_until(*socket_, *buffer_, "\r\n\r\n");
  
  // Consume the response headers to arrive at the content.
  std::string header;
  while (std::getline(response_stream, header) && header != "\r") {}
}

std::streamsize
http_source::read(char* s, std::streamsize size)
{
  if (size > buffer_->in_avail())
    boost::asio::read(*socket_, *buffer_,
		      boost::asio::transfer_at_least(size-buffer_->in_avail()));
  
  size_t n = std::min(size, buffer_->in_avail());
  std::copy(boost::asio::buffers_begin(buffer_->data()),
	    boost::asio::buffers_begin(buffer_->data())+n, s);
  buffer_->consume(n);
  
  return n;
}

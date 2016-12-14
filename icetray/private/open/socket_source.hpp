#ifndef SOCKET_SOURCE_HPP
#define SOCKET_SOURCE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/version.hpp>

boost::iostreams::file_descriptor_source
create_socket_source(const std::string filename){
  std::string port("1313");
  std::string host = filename.substr(strlen("socket://"));
  if (host.rfind(':') != std::string::npos) {
    port = host.substr(host.rfind(':')+1);
    host.resize(host.rfind(':'));
  }

  addrinfo *res;
  int error;

  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  error = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
  if (error)
    log_fatal("Host resolution error (%s:%s): %s", host.c_str(),
	      port.c_str(), gai_strerror(error));
  int s = -1;
  for (/* struct addrinfo *r = res */; res != NULL; res = res->ai_next) {
    s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s < 0)
      continue;

    if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
      close(s);
      s = -1;
      continue;
    }

    break;
  }
  if (res != NULL)
    freeaddrinfo(res);
  
  if (s < 0)
    log_fatal("Could not connect to %s:%s: %s",
	      host.c_str(),
	      port.c_str(),
	      strerror(errno));
  log_info("Connect to %s:%s opened successfully", host.c_str(), port.c_str());
#if BOOST_VERSION < 104400
  boost::iostreams::file_descriptor_source fs(s, true);
#else
  boost::iostreams::file_descriptor_source fs(s, boost::iostreams::close_handle);
#endif
  return fs;
}

#endif // SOCKET_SOURCE_HPP

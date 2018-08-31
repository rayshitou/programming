#pragma once
#include "vector.hpp"
#include "ip.hpp"

namespace fc
{
  std::vector<boost::asio::ip::udp::endpoint> resolve(boost::asio::io_service& io_service,
                                                      const std::string& host, uint16_t port);
}

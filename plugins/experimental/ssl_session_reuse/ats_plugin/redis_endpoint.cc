#include <sstream>
#include "redis_endpoint.h"

redis_endpoint::redis_endpoint(const std::string &endpoint_spec) : m_endpoint(endpoint_spec)
{
  std::stringstream ss;
  size_t delim_pos(endpoint_spec.find(':'));
  m_hostname = endpoint_spec.substr(0, delim_pos);

  if (m_hostname.empty()) {
    m_hostname = cDefaultRedisHost;
  }

  if (delim_pos != std::string::npos) {
    ss << endpoint_spec.substr(delim_pos + 1);
    ss >> m_port;
  } else {
    m_port = cDefaultRedisPort;
  }
}

std::ostream &
operator<<(std::ostream &os, const redis_endpoint &re)
{
  os << "RedisHost: " << re.m_hostname << " RedisPort: " << re.m_port << std::endl;
  return os;
}

void
addto_endpoint_vector(std::vector<RedisEndpoint> &endpoints, const std::string &endpoint_str)
{
  char delim(',');
  size_t current_start_pos(0);
  size_t current_end_pos(0);
  std::string current_endpoint;

  while ((std::string::npos != current_end_pos) && (current_start_pos < endpoint_str.size())) {
    current_end_pos = endpoint_str.find(delim, current_start_pos);

    if (std::string::npos != current_end_pos) {
      current_endpoint = endpoint_str.substr(current_start_pos, current_end_pos - current_start_pos);
    } else {
      current_endpoint = endpoint_str.substr(current_start_pos);
    }

    endpoints.push_back(RedisEndpoint(current_endpoint));

    current_start_pos = current_end_pos + 1;
  }
}

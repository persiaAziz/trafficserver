#ifndef _REDIS_ENDPOINT_H_
#define _REDIS_ENDPOINT_H_

#include <iostream>
#include <string>
#include <vector>
#include "globals.h"

typedef struct redis_endpoint {
  std::string m_hostname;
  int m_port;
  std::string m_endpoint;

  redis_endpoint() : m_hostname(cDefaultRedisHost), m_port(cDefaultRedisPort), m_endpoint(cDefaultRedisEndpoint) {}
  redis_endpoint(const std::string &endpoint_spec);

} RedisEndpoint;

typedef struct redis_endpoint_compare {
  bool
  operator()(const RedisEndpoint &lhs, const RedisEndpoint &rhs)
  {
    return lhs.m_endpoint < rhs.m_endpoint;
  }

} RedisEndpointCompare;

void addto_endpoint_vector(std::vector<RedisEndpoint> &endpoints, const std::string &endpoint_str);
std::ostream &operator<<(std::ostream &os, const redis_endpoint &re);

#endif

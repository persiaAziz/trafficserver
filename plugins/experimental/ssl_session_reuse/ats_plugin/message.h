#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>
#include <set>
#include "redis_endpoint.h"

typedef struct message {
  std::string channel;
  std::string data;
  bool cleanup;
  std::set<RedisEndpoint, RedisEndpointCompare> hosts_tried;

  message() {}
  message(const std::string &c, const std::string &d, bool quit = false) : channel(c), data(d), cleanup(quit) {}
  virtual ~message() {}

} Message;

#endif

#ifndef _SUBSCRIBER_H_
#define _SUBSCRIBER_H_

#include <queue>
#include <vector>
#include <string>
#include <semaphore.h>
#include <hiredis/hiredis.h>
#include "message.h"
#include "globals.h"
#include "redis_endpoint.h"

class RedisSubscriber
{
private:
  std::string redis_passwd;

  std::vector<RedisEndpoint> m_redisEndpoints;
  int m_redisEndpointsIndex;
  std::string m_channel;
  std::string m_channel_prefix;

  unsigned int m_redisConnectTimeout; // milliseconds
  unsigned int m_redisRetryDelay;     // milliseconds

  bool err;

  ::redisContext *setup_connection(int index);

  friend void *start(void *arg);

public:
  void run();
  RedisSubscriber(const std::string &conf = cDefaultConfig);
  virtual ~RedisSubscriber();
  bool is_good();
  int get_endpoint_index();
};

#endif

#ifndef _PUBLISHER_H_
#define _PUBLISHER_H_

#include <string>
#include <deque>
#include <vector>
#include <semaphore.h>
#include <hiredis/hiredis.h>
#include "message.h"
#include "globals.h"
#include "redis_endpoint.h"
#include "simple_pool.h"
#include "ts/HashFNV.h"

class RedisPublisher
{
  std::string redis_passwd;
  std::deque<Message> m_messageQueue;
  ::pthread_mutex_t m_messageQueueMutex;
  ::sem_t m_workerSem;

  std::vector<RedisEndpoint> m_redisEndpoints;
  std::string m_redisEndpointsStr;
  int m_endpointIndex;
  ::pthread_mutex_t m_endpointIndexMutex;

  std::vector<simple_pool *> pools;

  unsigned int m_numWorkers;
  unsigned int m_redisConnectTimeout; // milliseconds
  unsigned int m_redisConnectTries;
  unsigned int m_redisPublishTries;
  unsigned int m_redisRetryDelay; // milliseconds
  unsigned int m_maxQueuedMessages;
  unsigned int m_poolRedisConnectTimeout; // milliseconds
  unsigned int m_getSessionTimeout;       // milliseconds
  unsigned int m_sessionLifetime;         // seconds
  unsigned int m_slowlogThresholdTime;    // milliseconds
  unsigned int m_enableRedisStore;

  bool err;

  void runWorker();

  ::redisContext *setup_connection(const RedisEndpoint &re);
  void teardown_connection(::redisContext *context);

  ::redisReply *send_publish(::redisContext **ctx, const RedisEndpoint &re, const Message &msg);
  ::redisReply *set_session(const Message &msg);
  void clear_reply(::redisReply *reply);

  uint32_t
  get_hash_index(const std::string &str) const
  {
    ATSHash32FNV1a hashFNV;
    hashFNV.update(str.c_str(), str.length());
    return hashFNV.get();
  }

  uint32_t
  get_next_index(uint32_t index) const
  {
    return (index + 1) % m_redisEndpoints.size();
  }

  int signal_cleanup();
  friend void *start_worker_thread(void *arg);

public:
  RedisPublisher(const std::string &conf = cDefaultConfig);
  virtual ~RedisPublisher();
  int publish(const std::string &channel, const std::string &message);
  std::string get_session(const std::string &channel);
  bool is_good();
};

#endif
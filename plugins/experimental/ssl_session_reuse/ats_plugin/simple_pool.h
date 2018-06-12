#ifndef _SIMPLE_POOL_H
#define _SIMPLE_POOL_H

#include "connection.h"
#include <set>
#include <pthread.h>

/**
 * @brief Manages a pool of connections to a single Redis server
 */
class simple_pool
{
public:
  static inline simple_pool *
  create(const std::string &host = "localhost", unsigned int port = 6379, unsigned int timeout = 5)
  {
    return (new simple_pool(host, port, timeout));
  }

  /**
   * @brief Get a working connection
   * @return
   */
  connection *get();

  /**
   * @brief Put back a connection for reuse
   * @param conn
   */
  void put(connection *conn);

private:
  simple_pool(const std::string &host, unsigned int port, unsigned int timeout);

  std::string _host;
  unsigned int _port;
  unsigned int _timeout;
  std::set<connection *> connections;
  pthread_mutex_t access_mutex;
};

#endif

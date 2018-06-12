#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <string>
#include <hiredis/hiredis.h>

/**
 * @brief The connection class, represent a connection to a Redis server
 */
class connection
{
public:
  /**
   * @brief Create and open a new connection
   * @param host hostname or ip of redis server, default localhost
   * @param port port of redis server, default: 6379
   * @param timeout time out in milli-seconds, default: 5
   * @return
   */
  inline static connection *
  create(const std::string &host = "localhost", const unsigned int port = 6379, const unsigned int timeout = 5)
  {
    return (new connection(host, port, timeout));
  }

  ~connection();

  bool is_valid() const;

  /**
   * @brief Returns raw ptr to hiredis library connection.
   * Use it with caution and pay attention on memory
   * management.
   * @return
   */
  inline redisContext *const
  c_ptr() const
  {
    return c;
  }

private:
  friend class connection_pool;
  connection(const std::string &host, const unsigned int port, const unsigned int timeout);
  redisContext *c;
};

#endif

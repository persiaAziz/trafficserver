#include "connection.h"

connection::connection(const std::string &host, const unsigned int port, const unsigned int timeout)
{
  struct timeval tv;
  tv.tv_sec  = timeout / 1000;
  tv.tv_usec = (timeout % 1000) * 1000;

  c = redisConnectWithTimeout(host.c_str(), port, tv);
  if (c) {
    if (c->err != REDIS_OK) {
      redisFree(c);
      c = NULL;
    }
  }
}

connection::~connection()
{
  if (c) {
    redisFree(c);
  }
}

bool
connection::is_valid() const
{
  if (c) {
    return c->err == REDIS_OK;
  }
  return false;
}

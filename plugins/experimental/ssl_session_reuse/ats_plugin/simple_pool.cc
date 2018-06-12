#include "simple_pool.h"
#include "scope_lock.h"

connection *
simple_pool::get()
{
  connection *ret = NULL;

  {
    scope_lock sl(&access_mutex);
    for (std::set<connection *>::iterator it = connections.begin(); it != connections.end();) {
      if (*it) {
        if ((*it)->is_valid()) {
          ret = *it;
          connections.erase(it++);
          break;
        } else {
          delete *it;
          connections.erase(it++);
        }
      }
    }
  }

  if (ret == NULL) {
    ret = connection::create(_host, _port, _timeout);
    if (ret && !ret->is_valid()) {
      delete ret;
      ret = NULL;
    }
  }

  return ret;
}

void
simple_pool::put(connection *conn)
{
  if (conn == NULL)
    return;

  if (!conn->is_valid()) {
    delete conn;
    return;
  }

  {
    scope_lock sl(&access_mutex);
    connections.insert(conn);
  }
}

simple_pool::simple_pool(const std::string &host, unsigned int port, unsigned int timeout)
  : _host(host), _port(port), _timeout(timeout)
{
  pthread_mutex_init(&access_mutex, NULL);
}

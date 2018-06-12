#include "scope_lock.h"

scope_lock::scope_lock(pthread_mutex_t *mutex)
{
  m = mutex;

  if (m) {
    pthread_mutex_lock(m);
  }
}

scope_lock::~scope_lock()
{
  if (m) {
    pthread_mutex_unlock(m);
  }
}

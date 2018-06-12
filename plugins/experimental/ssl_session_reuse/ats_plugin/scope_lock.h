#ifndef _SCOPE_LOCK_H
#define _SCOPE_LOCK_H

#include <pthread.h>

/**
 * @brief Scoped lock works by locking a mutex when they are constructed,
 * and unlocking it when they are destructed.
 * The C++ rules guarantee that when control flow leaves a scope (even via an exception),
 * objects local to the scope being exited are destructed correctly.
 */
class scope_lock
{
public:
  scope_lock(pthread_mutex_t *mutex);
  ~scope_lock();

private:
  pthread_mutex_t *m;
};

#endif

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/* Original version Copyright (C) Scott Bilas, 2000.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */

#include <new>
template <typename _T> class Singleton
{
protected:
  static _T *ms_Singleton;
  Singleton() {}

public:
  ~Singleton(void)
  {
    if (ms_Singleton)
      delete ms_Singleton;
  }

  static _T &
  getSingleton(void)
  {
    if (!ms_Singleton) {
      ms_Singleton = new (std::nothrow) _T();
    }

    return (*ms_Singleton);
  }

  static _T *
  getSingletonPtr(void)
  {
    if (!ms_Singleton) {
      ms_Singleton = new (std::nothrow) _T();
    }

    return (ms_Singleton);
  }
};

#endif

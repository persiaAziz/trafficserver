#ifndef _SSL_UTILS_H_
#define _SSL_UTILS_H_

#include <openssl/ssl.h>
#include <string>
#include <ts/ts.h>

#include "publisher.h"
#include "subscriber.h"

#include "stek.h"

struct ssl_session_param {
  std::string app_name;
  std::string cluster_name;
  int key_type;
  int key_version;
  int key_update_interval; // STEK master rotation period seconds
  int stek_master;         // bool - Am I the STEK setter/rotator for POD?
  int enable_sessionId_cache;
  int64_t session_lifetime;
  ssl_ticket_key_t ticket_keys[2]; // current and past STEK
  std::string redis_auth_key_file;
  RedisPublisher *pub;
  RedisSubscriber *sub;

  ssl_session_param();
  ~ssl_session_param();
};

int STEK_init_keys();

const char *get_key_ptr();
int get_key_length();

/* Initialize ssl parameters */
/**
   Return the result of initialization. If 0 is returned, it means
   the initializtion is success, -1 means it is failure.

   @param conf_file the configuration file

   @return @c 0 if it is success.

*/
int init_ssl_params(const std::string &conf_file);
int init_subscriber();

int SSL_session_callback(TSCont contp, TSEvent event, void *edata);

extern ssl_session_param ssl_param; // almost everything one needs is stored in here

#endif

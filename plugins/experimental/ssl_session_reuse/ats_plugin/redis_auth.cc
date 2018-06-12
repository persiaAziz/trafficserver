#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ts/ts.h>
#include <unistd.h>
#include "ssl_utils.h"
#include <sys/stat.h>
#include <fcntl.h>


extern ssl_session_param ssl_param;
#define REDIS_KEY_NAME "redis_session_reuse"

/*
 Read the redis auth key from file ssl_param.redis_auth_key_file in retKeyBuff
 
 */
int
get_redis_auth_key(char *retKeyBuff, int buffSize)
{
  time_t currentTime;

  // Get the Key
  if (ssl_param.redis_auth_key_file.length()) {
    int fd = open(ssl_param.redis_auth_key_file.c_str(), O_RDONLY);
    struct stat info;
    if (0 == fstat(fd, &info)) {
      size_t n = info.st_size;
      std::string key_data;
      key_data.reserve(n);
      auto read_len = read(fd, key_data.data(), n);
      strncpy(retKeyBuff, key_data.c_str(), n);
    }
  } else {
    TSError("can not get redis auth key");
    return 0; /* error */
  }

  return 1; /* ok */
}

#ifndef _REDIS_AUTH_H
#define _REDIS_AUTH_H

#define MAX_REDIS_KEYSIZE 256

int get_redis_auth_key(char *retKeyBuff, int buffSize);

#endif

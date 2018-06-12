#ifndef _STEK_H_INCLUDED_BEFORE_
#define _STEK_H_INCLUDED_BEFORE_

/* STEK - Session Ticket Encryption Key stuff */

#define STEK_ID_NAME "stek" // ACTUALLY it is redis channel minus cluster_name prefix, aka mdbm keyname
#define STEK_ID_RESEND "resendstek"
#define STEK_MAX_LIFETIME 86400 // 24 hours max - should rotate STEK

#define STEK_NOT_CHANGED_WARNING_INTERVAL (2 * STEK_MAX_LIFETIME) // warn on non-stek rotate every X secs.

#define SSL_KEY_LEN 16

struct ssl_ticket_key_t // an STEK
{
  unsigned char key_name[SSL_KEY_LEN]; // tickets use this name to identify who encrypted
  unsigned char hmac_secret[SSL_KEY_LEN];
  unsigned char aes_key[SSL_KEY_LEN];
};

void STEK_update(const std::string &encrypted_stek);
bool isSTEKMaster();
int STEK_Send_To_Network(struct ssl_ticket_key_t *stekToSend);

#endif /* _STEK_H_INCLUDED_BEFORE_ */

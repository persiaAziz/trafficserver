#ifndef _SESSION_PROCESS_H_
#define _SESSION_PROCESS_H_

#include <string>
#include <math.h>

#define SSL_SESSION_MAX_DER 1024 * 10

// Base 64 encoding takes 4*(floor(n/3)) bytes
#define ENCODED_LEN(len) ((int)ceil(1.34 * len + 5)) + 1
#define DECODED_LEN(len) ((int)ceil(0.75 * len)) + 1
// 3DES encryption will take at most 8 extra bytes.  Plus we base 64 encode the result
#define ENCRYPT_LEN(len) (int)ceil(1.34 * (len + 8) + 5) + 1
#define DECRYPT_LEN(len) (int)ceil(1.34 * (len + 8) + 5) + 1

int encrypt_session(const char *session_data, int32_t session_data_len, const char *key, int key_length,
                    std::string &encrypted_data);

int decrypt_session(const std::string &encrypted_data, const char *key, int key_length, char *session_data, int32_t &session_len);

int encode_id(const char *id, int idlen, std::string &decoded_data);
int decode_id(std::string encoded_id, char *decoded_data, int &decoded_data_len);

int add_session(char *session_id, int session_id_len, const std::string &encrypted_session);

#endif

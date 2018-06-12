//#include <yax/y64.h>
//#include <ycrypto/ycr/yCryptoEncrypt.h>
//#include <ycrypto/ycr/yCrypto.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <ts/ts.h>

#include "session_process.h"
#include "ssl_utils.h"

#include "common.h"

static const unsigned char salt[] = {115, 97, 108, 117, 0, 85, 137, 229};
int
encrypt_session(const char *session_data, int32_t session_data_len, const unsigned char *key, int key_length,
                std::string &encrypted_data)
{
  size_t len_all          = 0;
  size_t offset           = 0;
  char *pBuf              = nullptr;
  EVP_CIPHER_CTX *context = nullptr;

  int encrypted_buffer_size    = 0;
  int encrypted_msg_len        = 0;
  unsigned char *encrypted_msg = NULL;
  int ret                      = 0;

  offset  = 0;
  len_all = sizeof(int64_t) + sizeof(int32_t) + session_data_len;

  pBuf = new char[len_all];

  // Put in a fixed experation time of 7 hours, just to have communication consistency with the original
  // protocol
  int64_t expire_time = time(NULL) + 2 * 3600;
  memcpy(pBuf + offset, &expire_time, sizeof(expire_time));
  offset += sizeof(int64_t);

  memcpy(pBuf + offset, &session_data_len, sizeof(int32_t));
  offset += sizeof(session_data_len);
  memcpy(pBuf + offset, session_data, session_data_len);

  // Initialize context
  context = EVP_CIPHER_CTX_new();
  unsigned char iv[EVP_MAX_IV_LENGTH];
  unsigned char gen_key[EVP_MAX_KEY_LENGTH];

  // generate key and iv
  int generated_key_len = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), (unsigned char *)salt, key, key_length, 1, gen_key, iv);
  if (generated_key_len <= 0) {
    TSDebug(PLUGIN, "Error generating key");
  }

  // Set context AES128 with the generated key and iv
  if (1 != EVP_EncryptInit_ex(context, EVP_aes_256_cbc(), nullptr, gen_key, iv)) {
    TSDebug(PLUGIN, "Encryption of session data failed");
    ret = -1;
    goto Cleanup;
  }

  //    if (ycrCreateSymmetricContext(&context, key, key_length) != YC_OK) {
  //        TSError( "ycrCreateSymmetricContext failed.");
  //        ret = -1;
  //        goto Cleanup;
  //    }
  int elen;
  encrypted_buffer_size = ENCRYPT_LEN(len_all);
  encrypted_msg_len     = encrypted_buffer_size;
  encrypted_msg         = new unsigned char[encrypted_buffer_size];
  if (1 != EVP_EncryptUpdate(context, (unsigned char *)encrypted_msg, &elen, (unsigned char *)pBuf, len_all)) {
    TSDebug(PLUGIN, "Encryption of session data failed");
    ret = -1;
    goto Cleanup;
  }
  encrypted_msg_len = elen;
  if (1 != EVP_EncryptFinal_ex(context, encrypted_msg + elen, &elen)) {
    TSDebug(PLUGIN, "Encryption of session data failed");
    ret = -1;
    goto Cleanup;
  }
  encrypted_msg_len += elen;
  // encrypt with 3DES and attach MD5 hash
  //    err = ycrEncryptSign64(context, (const char*)pBuf, len_all, (char *)encrypted_msg, &encrypted_msg_len, NULL, 0);
  //    if (err == YC_ERROR || encrypted_msg_len <= 0) {
  //        TSError("ycrEncryptSign64 failed.");
  //        ret = -1;
  //        goto Cleanup;
  //    }

  encrypted_data.assign((char *)encrypted_msg, encrypted_msg_len);

Cleanup:

  if (pBuf)
    delete[] pBuf;
  if (encrypted_msg)
    delete[] encrypted_msg;
  if (context) {
    EVP_CIPHER_CTX_free(context);
  }

  return ret;
}

/**
 * The initial value of session_data_len is the number of bytes in session_data
 * The return value of session_data_len is the number of bytes actually stored in session_data
 * The return value is -1 on error or the number of bytes in the decrypted session data (may be more
 * than the initial value of sesion_data_len
 */
int
decrypt_session(const std::string &encrypted_data, const unsigned char *key, int key_length, char *session_data,
                int32_t &session_data_len)
{
  unsigned char *ssl_sess_ptr  = nullptr;
  EVP_CIPHER_CTX *context      = nullptr;
  int decrypted_buffer_size    = 0;
  int decrypted_msg_len        = 0;
  unsigned char *decrypted_msg = nullptr;
  int ret                      = 0;

  // Initialize context
  // Initialize context
  context = EVP_CIPHER_CTX_new();
  unsigned char iv[EVP_MAX_IV_LENGTH];
  unsigned char gen_key[EVP_MAX_KEY_LENGTH];

  // generate key and iv
  int generated_key_len = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), (unsigned char *)salt, key, key_length, 1, gen_key, iv);

  if (generated_key_len <= 0) {
    TSDebug(PLUGIN, "Error generating key");
  }
  // set context with the generated key and iv
  if (1 != EVP_DecryptInit_ex(context, EVP_aes_256_cbc(), NULL, gen_key, iv)) {
    TSDebug(PLUGIN, "Decryption of encrypted session data failed");
    goto Cleanup;
  }
  // Decryption
  //  if (ycrCreateSymmetricContext(&context, key, key_length) != YC_OK) {
  //    TSError("ycrCreateSymmetricContext failed.");
  //    ret = -1;
  //    goto Cleanup;
  //  }

  decrypted_buffer_size = DECRYPT_LEN(encrypted_data.length());
  decrypted_msg         = (unsigned char *)new char[decrypted_buffer_size + 1];
  decrypted_msg_len     = decrypted_buffer_size + 1;
  if (decrypted_msg == NULL)
    TSError("decrypted_msg allocate failure");
  if (1 != EVP_DecryptUpdate(context, decrypted_msg, &decrypted_msg_len, (unsigned char *)encrypted_data.c_str(),
                             encrypted_data.length())) {
    TSDebug(PLUGIN, "Decryption of encrypted session data failed");
    goto Cleanup;
  }
  // err = ycrDecryptSign64(context, encrypted_data.c_str(), encrypted_data.length(), decrypted_msg, &decrypted_msg_len, NULL, 0);
  //  if (err == YC_ERROR || decrypted_msg_len <= 0) {
  //    TSError("ycrDecryptSign64 failed.");
  //    TSDebug(PLUGIN,
  //            "ycrDecryptSign64 failed. error: %s, decrypted_buffer_size:%d; (cipher-text) encrypted_data.length():%d, "
  //            "encrypted_data.c_str(): %s",
  //            strerror(errno), (int)decrypted_buffer_size, (int)encrypted_data.length(), encrypted_data.c_str());
  //    /* if the above values look good, then it's possible key (for context) is wrong */
  //    ret = -1;
  //    goto Cleanup;
  //  }

  // Retrieve ssl_session
  ssl_sess_ptr = (unsigned char *)decrypted_msg;

  // Skip the expiration time.  Just a place holder to interact with the old version
  ssl_sess_ptr += sizeof(int64_t);

  // Length
  ret = *(int32_t *)ssl_sess_ptr;
  ssl_sess_ptr += sizeof(int32_t);
  // If there is less data than the maxiumum buffer size, reduce accordingly
  if (ret < session_data_len) {
    session_data_len = ret;
  }
  memcpy(session_data, ssl_sess_ptr, session_data_len);

Cleanup:

  if (decrypted_msg)
    delete[] decrypted_msg;

  if (context) {
    EVP_CIPHER_CTX_free(context);
  }

  return ret;
}

int
decode_id(std::string encoded_id, char *decoded_data, int &decoded_data_len)
{
  size_t decode_len = 0;
  memset(decoded_data, 0, decoded_data_len);
  if (TSBase64Decode((const char *)encoded_id.c_str(), encoded_id.length(), (unsigned char *)decoded_data, decoded_data_len,
                     &decode_len) != 0) {
    TSError("Base 64 decoding failed.");
    return -1;
  }
  return 0;
}

int
encode_id(const char *id, int idlen, std::string &encoded_data)
{
  char *encoded = new char[ENCODED_LEN(idlen)];
  memset(encoded, 0, ENCODED_LEN(idlen));
  size_t encoded_len = 0;
  if (TSBase64Encode((const char *)id, idlen, encoded, ENCODED_LEN(idlen), &encoded_len) != 0) {
    TSError("Base 64 encoding failed.");
    if (encoded)
      delete[] encoded;
    return -1;
  }

  encoded_data.assign(encoded);
  if (encoded)
    delete[] encoded;

  return 0;
}

int
add_session(char *session_id, int session_id_len, const std::string &encrypted_session)
{
  char session_data[SSL_SESSION_MAX_DER];
  int32_t session_data_len = SSL_SESSION_MAX_DER;
  if (decrypt_session(encrypted_session, get_key_ptr(), get_key_length(), session_data, session_data_len) < 0) {
    TSError("Failed to decrypt session %.*s", session_id_len, session_id);
    return -1;
  }
  const unsigned char *loc = reinterpret_cast<const unsigned char *>(session_data);
  SSL_SESSION *sess        = d2i_SSL_SESSION(NULL, &loc, session_data_len);
  if (NULL == sess) {
    TSError("Failed to transform session buffer %.*s", session_id_len, session_id);
  }
  TSSslSessionID sid;
  memcpy(reinterpret_cast<char *>(sid.bytes), session_id, session_id_len);
  sid.len = session_id_len;
  if (sid.len > sizeof(sid.bytes)) {
    sid.len = sizeof(sid.bytes);
  }
  TSSslSessionInsert(&sid, reinterpret_cast<TSSslSession>(sess));
  // Free the sesison object created by d2i_SSL_SESSION
  // We should make an API that just takes the ASN buffer
  SSL_SESSION_free(sess);
  return 0;
}

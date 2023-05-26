#include "CanGlobal.h"

char dic_table[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()-_+=[]{}\"':;<>,./?\\|~`";

authToken::authToken(const char *remoteKey, const char *carKey)
{
  if (remoteKey)
    memcpy(_remoteKey, remoteKey, sizeof(*_remoteKey) * AUTH_TOKEN_SIZE);
  if (carKey)
    memcpy(_carKey, carKey, sizeof(*_carKey) * AUTH_TOKEN_SIZE);
  memset(_token, 0, sizeof(*_token) * AUTH_TOKEN_SIZE);
  memset(_challenge, 0, sizeof(*_challenge) * AUTH_TOKEN_SIZE);
}

authToken::~authToken()
{

}

char *
authToken::carKey(const char *key)
{
  if (key)
    memcpy(_carKey, key, sizeof(*_carKey) * AUTH_TOKEN_SIZE);
  return (_carKey);
}

char *
authToken::remoteKey(const char *key)
{
  if (key)
    memcpy(_remoteKey, key, sizeof(*_remoteKey) * AUTH_TOKEN_SIZE);
  return (_carKey);
}

void
authToken::resetToken()
{
  memset(_token, 0, sizeof(*_token) * AUTH_TOKEN_SIZE);
}

void
authToken::resetChallenge()
{
  memset(_challenge, 0, sizeof(*_challenge) * AUTH_TOKEN_SIZE);
}

char *
authToken::genToken()
{
  unsigned int	i;
  unsigned int	dic_len;

  dic_len = strlen(dic_table);
  for (i = 0; i < AUTH_TOKEN_SIZE; i++)
    _token[i] = dic_table[(millis()*random()) % dic_len];
  return (_token);
}

char *
authToken::setToken(const char *token)
{
  memcpy(_token, token, sizeof(*_token) * AUTH_TOKEN_SIZE);
  return (_token);
}

char *
authToken::setChallenge(const char *challenge)
{
  memcpy(_challenge, challenge, sizeof(*_challenge) * AUTH_TOKEN_SIZE);
  return (_challenge);
}


void
authToken::encryptWithCar()
{	
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) _carKey, AUTH_TOKEN_SIZE * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)_token, (unsigned char*)_challenge);
  mbedtls_aes_free(&aes); 
}

void
authToken::decryptWithCar()
{
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) _carKey, AUTH_TOKEN_SIZE * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)_challenge, (unsigned char*)_token);
  mbedtls_aes_free(&aes);
}

void
authToken::encryptWithRemote()
{
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) _remoteKey, AUTH_TOKEN_SIZE * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)_token, (unsigned char*)_challenge);
  mbedtls_aes_free(&aes);
}

void
authToken::decryptWithRemote()
{
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*) _remoteKey, AUTH_TOKEN_SIZE * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)_challenge, (unsigned char*)_token);
  mbedtls_aes_free(&aes);
}

#ifndef __AUTHTOKEN_H__
# define __AUTHTOKEN_H__

# define AUTH_TOKEN_SIZE	16

class	authToken
{
 public:
  authToken(const char *remoteKey = 0, const char *carKey = 0);
  ~authToken();
  char *	carKey(const char *key = 0);
  char *	remoteKey(const char *key = 0);
  char *	token() { return (_token); }
  char *	challenge() { return (_challenge); }
  char *	copyToken(char *copy) { memcpy(copy, _token, sizeof(*copy) * AUTH_TOKEN_SIZE); return (_token); }
  char *	copyChallenge(char *copy) { memcpy(copy, _challenge, sizeof(*copy) * AUTH_TOKEN_SIZE); return (_challenge); }

  void		resetToken();
  void		resetChallenge();
  char *	genToken();
  char *       	setToken(const char *token);
  char *	setChallenge(const char *challenge);
  void		encryptWithCar();
  void		decryptWithCar();
  void		encryptWithRemote();
  void		decryptWithRemote();

private:
  unsigned long nounce1;
  char		_carKey[AUTH_TOKEN_SIZE];
  char		_remoteKey[AUTH_TOKEN_SIZE];  
  char		_token[AUTH_TOKEN_SIZE];
  char		_challenge[AUTH_TOKEN_SIZE];
};

#endif  // !__AUTHTOKEN_H__ //

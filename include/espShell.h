#ifndef __ESPSHELL_H__
# define __ESPSHELL_H__

// right now SPIFFS limits name len to 32 ... but keeping some room for potential future updates ^^
// 256 should be enough ... (dos limit ?)
# define MAX_SPIFFS_NAME_LEN		256

class espShell;

# include "tools.h"

typedef struct	s_cmdfunc
{
  const char	        *name;
  const char		*help;
  bool		(*fct)(espShell *sh, Stream *s, char **args);
}		t_cmdfunc;

typedef struct		s_menulist
{
  const char			*name;
  const char			*help;
  t_cmdfunc		*cmdfunc;
  struct s_menulist	*next;
}			t_menulist;

class espShell
{
 public:
  espShell(const char *name, Stream *s, bool echo = true, bool interactive = true, bool secure = false);
  ~espShell();
  bool			authCheck();
  void			checkCmdLine();
  Stream *		cmdSerial()  { return (_serial); }
  void			init();
  const char *		path(const char *newpath = 0);
  Stream *		serial() { return (_serial); }
  bool			getYesNo(const char *Message);
  void			secure(bool secure) { _secure = secure; }
  bool			secure() { return (_secure); }
  void			interactive(bool interactive) { _interactive = interactive; }
  bool			interactive() { return (_interactive); }
  void			echo(bool echo) { _echo = echo; }
  bool			echo() { return (_echo); }
  void			convertAliases();
  bool			runLine(const char *line);

private:
  bool			_readLine();
  void			_clearLine();
  void			_prompt();
  bool			_interpreteLine();
  String		_name;
  Stream *		_serial;
  char *		_line;
  short			_endline;
  short			_linelen;
  bool			_interactive;
  bool			_echo;
  bool			_secure;
  t_menulist *		_actualmenu;
  char			_currentPath[MAX_SPIFFS_NAME_LEN];
};

#endif // !__ESPSHELL_H__ //

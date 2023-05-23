#ifndef __ESPSHELL_H__
# define __ESPSHELL_H__

// right now SPIFFS limits name len to 32 ... but keeping some room for potential future updates ^^
// 256 should be enough ... (dos limit ?)
# define MAX_SPIFFS_NAME_LEN		256

class espShell;

bool	cat(espShell *sh, Stream *s, char **args);
bool	ls(espShell *sh, Stream *s, char **args);
bool    rm(espShell *sh, Stream *s, char **args);
bool    cp(espShell *sh, Stream *s, char **args);
bool    mv(espShell *sh, Stream *s, char **args);
bool    ed(espShell *sh, Stream *s, char **args);
bool    md5sum(espShell *sh, Stream *s, char **args);
bool    xmreceive(espShell *sh, Stream *s, char **args);
//bool    hexrecv(espShell *sh, Stream *s, char **args);
bool    candump(espShell *sh, Stream *s, char **args);
bool    canwait(espShell *sh, Stream *s, char **args);
bool    canwrite(espShell *sh, Stream *s, char **args);
bool    canwritefile(espShell *sh, Stream *s, char **args);
bool    fwupdate(espShell *sh, Stream *s, char **args);
bool    interactive(espShell *sh, Stream *s, char **args);
bool    ifconfig(espShell *sh, Stream *s, char **args);
bool    lorasend(espShell *sh, Stream *s, char **args);
bool    lorasecure(espShell *sh, Stream *s, char **args);
bool    cfg(espShell *sh, Stream *s, char **args);
bool    alias(espShell *sh, Stream *s, char **args);
bool    pin(espShell *sh, Stream *s, char **args);
bool    serial(espShell *sh, Stream *s, char **args);
bool    exec(espShell *sh, Stream *s, char **args);
bool    gps(espShell *sh, Stream *s, char **args);
bool    cmd_delay(espShell *sh, Stream *s, char **args);
bool    cmd_sleep(espShell *sh, Stream *s, char **args);
bool    cmd_free(espShell *sh, Stream *s, char **args);
bool    echo(espShell *sh, Stream *s, char **args);
bool    restart(espShell *sh, Stream *s, char **args);
const char * getfullpath(const char *ptr);
void    showbuff(Stream *s, const char *title, const char *buff);
void    showbuffhex(Stream *s, const char *title, const char *buff);

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
  espShell(Stream *s, bool echo = true, bool secure = false);
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
  void			interactive(bool echo) { _echo = echo; }
  bool			interactive() { return (_echo); }
  void			convertAliases();
  bool			runLine(char *line);

private:
  bool			_readLine();
  void			_clearLine();
  void			_prompt();
  bool			_interpreteLine();
  Stream *		_serial;
  char *		_line;
  short			_endline;
  short			_linelen;
  bool			_echo;
  bool			_secure;
  t_menulist *		_actualmenu;
  char			_currentPath[MAX_SPIFFS_NAME_LEN];
};

#endif // !__ESPSHELL_H__ //

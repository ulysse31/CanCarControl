#include "CanGlobal.h"

#include "gl_commands.h"

espShell::espShell(const char *name, Stream *s, bool echo, bool interactive, bool secure)
{
  _serial = s;
  _line = 0;
  _endline = 0;
  _linelen = 16;
  _interactive = interactive;
  _echo = echo;
  _secure = secure;
  _name = name;
  _line = (char *)(xmalloc(sizeof(*_line) * (_linelen + 1)));
  memset(_line, 0, _linelen);
  memset(_currentPath, 0, MAX_SPIFFS_NAME_LEN);
  _currentPath[0] = '/'; // where all starts ...
}

espShell::~espShell()
{
  if (_line)
    free(_line);
}

void
espShell::init()
{
  this->_prompt();
}

const char *
espShell::path(const char *newpath)
{
  if (newpath)
    {
      if (strlen(newpath) > MAX_SPIFFS_NAME_LEN)
	return (0);
      memset(_currentPath, 0, MAX_SPIFFS_NAME_LEN);
      memcpy(_currentPath, newpath, strlen(newpath) * sizeof(*newpath));
    }
  return (_currentPath);
}

bool
espShell::getYesNo(const char *Message)
{
  char c;

  _serial->print(Message);
  _serial->print(" (Y/N): ");
  while (true)
    {
      if (_serial->available() > 0)
      {
        c = _serial->read();
        if (c == 'y' || c == 'Y')
          return (true);
        if (c == 'n' || c == 'N')
          return (false);
      }
      else
        delay(200);
    }
  return (false);
}

bool
espShell::authCheck()
{
  unsigned long	time;
  unsigned int	i;
  char		nounce[AUTH_TOKEN_SIZE];
  char		buff[AUTH_TOKEN_SIZE];
  authToken	car;

  if (CanCfg.getValue("LoraRemoteKey") == "" || CanCfg.getValue("LoraCarKey") == ""
      || CanCfg.getValue("LoraRemoteKey").length() != AUTH_TOKEN_SIZE
      || CanCfg.getValue("LoraCarKey").length() != AUTH_TOKEN_SIZE)
    return (false);

  car.carKey(CanCfg.getcValue("LoraCarKey"));
  car.remoteKey(CanCfg.getcValue("LoraRemoteKey"));
  car.genToken();
  car.copyToken(nounce);
  //showbuff(&Serial, "\nDEBUG: token:", nounce);
  car.encryptWithCar();
  //showbuffhex(&Serial, "\nDEBUG: sending challenge:", car.challenge());
  while (_serial->available())   // flushing any remaining bytes
    _serial->read();
  _serial->write(car.challenge(), AUTH_TOKEN_SIZE);
  time = millis();
  i = 0;
  while (true)
    if (_serial->available())
      {
        buff[i] = ((char)_serial->read());
	i++;
        if (i == AUTH_TOKEN_SIZE)
          break ;
        time = millis();
      }
    else
      {
        if ((millis() - time) > 5000) // keep in mind that if command takes more than this value without writing data ... connection will be closed
          {
	    _serial->println("KO");
	    return (false);
          }
      }
  //showbuffhex(&Serial, "\nDEBUG: Received:", buff);
  car.setChallenge(buff);
  car.decryptWithRemote();
  //showbuff(&Serial, "\nDEBUG: decrypt:", car.token());
  if (memcmp(nounce, car.token(), sizeof(*nounce) * AUTH_TOKEN_SIZE) == 0)
    return (true);
  //_serial->println("KO");
  return (false);
}

void
espShell::checkCmdLine()
{
  if (this->_readLine())
    {
      if (_secure == false || (this->_endline > 0 && this->authCheck()))
	{
	  if (_echo)
	    _serial->println("");
	  else
	    if (!_interactive)
	      _serial->write(0x04);
	  this->convertAliases();
	  this->_interpreteLine();
	}
      this->_clearLine();
      this->_prompt();
    }
}


bool
espShell::runLine(const char *line)
{
  bool ret;

  if (this->_line)
    {
      free(_line);
      _line = 0;
    }
  this->_line = (char *)xmalloc(sizeof(*_line) * (strlen(line) + 2));
  memset(_line, 0, sizeof(*_line) * (strlen(line) + 2));
  strcpy(_line, line);
  this->_endline = strlen(line);
  this->_linelen = (strlen(line) + 1);
  ret = this->_interpreteLine();
  this->_clearLine();
  return (ret);
}

bool
espShell::_readLine()
{ 
  while (_serial->available())
    {
      _line[_endline] = _serial->read();
      LastActivity = millis();
      if (_endline == 0 && _line[_endline] == '\n')
	  return (false);
      if (_endline == 0 && _line[_endline] == 0x08)
	{
	  _line[_endline] = 0;
	  return (false);
	}
      if (_line[_endline] == 0x08 && _endline > 0)
	{
	  if (_interactive)
	    {
	      _serial->print('\b');
	      _serial->print(' ');
	      _serial->print('\b');
	    }
	  _line[_endline] = 0;
	  _endline = _endline - 1;
	  return (false);
	}
      if (_echo)
	_serial->print(_line[_endline]);
      if (_line[_endline] == '\r' || _line[_endline] == '\n')
	{
	  _line[_endline] = 0;
	  return (true);
	}
      _endline++;
      if (_endline >= _linelen)
	{
	  _linelen *= 2;
	  char *tmp = (char *)(xmalloc(sizeof(*tmp) * (_linelen + 1)));
	  memset(tmp, 0, _linelen);
	  memcpy(tmp, _line, sizeof(*_line) * (_endline));
	  free(_line);
	  this->_line = tmp;
	}
    }
  return (false);
}

void
espShell::convertAliases()
{
  const char *	alias;
  const char *	cmd;
  char *	str;
  String	tmp;
  cfgNode       *p;
  int		i;
  unsigned int	aliasl;
  unsigned int	cmdl;
  unsigned int	strl;

  for (p = Aliases.startNode(); p; p = p->next())
    {
      tmp = p->key();
      alias = tmp.c_str();
      str = this->_line;
      while (*str == ' ' || *str == '\t')
	str++;
      strl = strlen(str);
      aliasl = strlen(alias);
      if (aliasl > 0 && strncmp(str, alias, aliasl) == 0
	  && (str[aliasl] == ' ' || str[aliasl] == 0))
	{
	  tmp = p->value();
	  cmd = tmp.c_str();
	  cmdl = strlen(cmd);
	  // free old line, allocate new + rest, copy new + rest of old
	  unsigned int newlen = cmdl + (strl - aliasl);
	  char *newline = (char *)xmalloc(sizeof(*newline) * (newlen + 2));
	  memset(newline, 0, sizeof(*newline) * (newlen + 2));
	  memcpy(newline, cmd, cmdl);
	  memcpy(newline + cmdl, str + aliasl, strl - aliasl);
	  free(_line);
	  _line = newline;
	  _linelen = newlen + 2;
	  _endline = newlen;
	  break ;
	}
    }
}

void
espShell::_clearLine()
{
  if (_line)
    free(_line);
  _line = 0;
  _linelen = 16;
  _endline = 0;
  _line = (char *)(xmalloc(sizeof(*_line) * (_linelen + 1)));
  memset(_line, 0, _linelen);
}

void
espShell::_prompt()
{
  if (_interactive)
    {
      _serial->print("[");
      _serial->print(_name);
      _serial->print(": ");
      _serial->print(_currentPath);
      _serial->print(" ]> ");
    }
}

bool
espShell::_interpreteLine()
{
  int		i;
  int		c;
  char		**args;
  t_cmdfunc	*f;
  bool		found = false;
  bool		quote = false;
  
  if (this->_endline <= 1)
    return (false);
  for (i = 0, c = 0; i <= this->_endline; i++)
    {
      if (this->_line[i] == '"')
	{
	  this->_line[i] = 0;
	  quote = (quote ? false : true);
	}
      if ((this->_line[i] == ' ' || this->_line[i] == '\r' || this->_line[i] == '\n' || this->_line[i] == '\t') && quote == false)
	{
	  this->_line[i] = 0;
	  c++;
	}
    }
  if (quote)
    {
      _serial->println("Syntax Error: Unmatched quote");
      return (false);
    }
  args = (char **)(xmalloc(sizeof(*args) * (c + 2)));
  memset(args, 0, sizeof(*args) * (c + 2));
  for (i = 0; this->_line[i] == 0 && i <= this->_endline; i++)
    ;
  args[0] = ((this->_line) + i);
  for (i = 0, c = 1; i <= this->_endline; i++)
    if (this->_line[i] == 0 && this->_line[i + 1])
      {
	args[c] = ((this->_line) + i + 1);
	c++;
      }
  for (f = gl_commands; f && f->name; f++)
    if ((i = strcmp(f->name, args[0])) == 0)
      {
	f->fct(this, this->_serial, args);
	if (_interactive == false)
	  this->_serial->write(0x04);
	LastActivity = millis();
	found = true;
	break ;
      }
  if (strcmp(args[0], "list") == 0 || strcmp(args[0], "help") == 0)
    {
      _serial->println("EspSH Command list: ");
      _serial->println("");
      for (f = gl_commands; f && f->name; f++)
	{
	  _serial->print(f->name);
	  _serial->print(": ");
	  _serial->println(f->help);
	}
      _serial->println("");
      found = true;
    }
  if (!(found))
    {
      _serial->print(args[0]);
      _serial->print(": ");
      _serial->println("Command not found.");
      if (_interactive == false)
        this->_serial->write(0x04);
    }
  free(args);
  return (true);
}

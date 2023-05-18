#include "CanCarControl.h"

t_cmdfunc       gl_commands[] = {
  { "cat", "show file content", cat },
  { "ls", "list files", ls },
  { "rm", "delete file", rm },
  { "cp", "copy file", cp },
  { "mv", "rename file", cp },
  { "ed", "a line-per-line basic text editor", ed },  
  { "md5sum", "calculates md5 of a file", md5sum },
  { "xmreceive", "receive file from tty using XMODEM protocol", xmreceive },
  { "candump", "CAN Dumper", candump },
  { "canwait", "CAN packet waiting for specific identifiers", canwait },
  { "canwrite", "CAN packet writing: from arguments", canwrite },
  { "fwupdate", "firmware update command", fwupdate },
  { "ifconfig", "show network connection states", ifconfig },
  { "interactive", "enable/disable interactive shell parameters (echo, verbose...)", interactive },
  { "cfg", "main config parameters editor", cfg },
  { "alias", "alias command / shortcuts", alias },
  { "pin", "pin input/output control", pin },
  { "sleep", "sleep status/management command", cmd_sleep },
  { "serial", "serial redirection tool", serial },
  { "exec", "execute commands from file", exec },
  { "delay", "sleeps for a specified number of milliseconds", cmd_delay },
  { "lorasend", "send command via lora", lorasend },
  { "lorasecure", "enable/disable auth in lorashell", lorasecure },
  { "restart", "restart the device", restart },
  {0, 0, 0}
};


espShell::espShell(Stream *s, bool echo, bool secure)
{
  _serial = s;
  _line = 0;
  _endline = 0;
  _linelen = 16;
  _echo = echo;
  _secure = secure;
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

  if (CanCarCfg.getValue("LoraRemoteKey") == "" || CanCarCfg.getValue("LoraCarKey") == ""
      || CanCarCfg.getValue("LoraRemoteKey").length() != AUTH_TOKEN_SIZE
      || CanCarCfg.getValue("LoraCarKey").length() != AUTH_TOKEN_SIZE)
    return (false);

  car.carKey(CanCarCfg.getcValue("LoraCarKey"));
  car.remoteKey(CanCarCfg.getcValue("LoraRemoteKey"));
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
	    _serial->write(0x04);
	  this->convertAliases();
	  this->_interpreteLine();
	}
      this->_clearLine();
      this->_prompt();
    }
}


bool
espShell::runLine(char *line)
{
  
  if (this->_line)
    {
      free(_line);
      _line = 0;
    }
  this->_line = (char *)xmalloc(sizeof(*_line) * (strlen(line) + 1));
  memset(_line, 0, sizeof(*_line) * (strlen(line) + 1));
  strcpy(_line, line);
  this->_endline = strlen(line);
  this->_linelen = (strlen(line) + 1);
  return (this->_interpreteLine());
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
	  if (_echo)
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
  if (_echo)
    {
      _serial->print("[CarSH: ");
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
	if (_echo == false)
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
    }
  free(args);
  return (true);
}

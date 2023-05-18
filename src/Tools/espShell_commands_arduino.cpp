#include "CanCarControl.h"

typedef struct	s_genericcmd
{
  const char *	name;
  bool		(*fct)(Stream *s, char **args);
}		t_genericcmd;

bool		pin_mode(Stream *s, char **args)
{
  String	pin;
  
  if (args[2] == 0 || (args[2] && args[3] == 0))
    {
      s->println("Error: pin mode <pin_num> <INPUT|INPUT_PULLUP|OUTPUT>");
      return (false);
    }
  if (strcmp(args[3], "INPUT") != 0 && strcmp(args[3], "INPUT_PULLUP") != 0 && strcmp(args[3], "OUTPUT") != 0)
    {
      s->println("Error: Only use INPUT or OUTPUT mode type");
      return (false);
    }
  pin = args[2];
  pinMode(pin.toInt(), (strcmp(args[3], "INPUT") == 0 ? INPUT : (strcmp(args[3], "OUTPUT") == 0 ? OUTPUT : INPUT_PULLUP)));
  return (true);
}

bool		pin_read(Stream *s, char **args)
{
  String	pin;
  
  if (args[2] == 0)
    {
      s->println("Error: pin read <pin_num>");
      return (false);
    }
  pin = args[2];
  s->print("Pin ");
  s->print(pin.toInt());
  s->print(" is ");
  s->println((digitalRead(pin.toInt()) == HIGH ? "HIGH" : "LOW"));
  return (true);
}

bool		pin_write(Stream *s, char **args)
{
  String	pin;
  
  if (args[2] == 0 || (args[2] && args[3] == 0))
    {
      s->println("Error: pin write <pin_num> HIGH|LOW");
      return (false);
    }
  if (strcmp(args[3], "HIGH") != 0 && strcmp(args[3], "LOW") != 0)
    {
      s->println("Error: state to write must be HIGH or LOW");
      return (false);
    }
  pin = args[2];
  digitalWrite(pin.toInt(), (strcmp(args[3], "HIGH") == 0 ? HIGH : LOW));
  return (true);
}

t_genericcmd	gl_pincmd[] =
  {
    {"mode", pin_mode},
    {"read", pin_read},
    {"write", pin_write},
    {0, 0}
  };


bool	pin(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: pin [ mode <n> <INPUT|INPUT_PULLUP|OUTPUT> | read <n> | write <n> <HIGH|LOW> ]");
      return (false);
    }
  for (int i = 0; gl_pincmd[i].name; i++)
    if (strcmp(args[1], gl_pincmd[i].name) == 0)
      return(gl_pincmd[i].fct(s, args));
  s->println("Usage: pin [ mode <n> <INPUT|INPUT_PULLUP|OUTPUT> | read <n> | write <n> <HIGH|LOW> ]");
  return (false);
}

typedef struct		s_seriallist
{
  const char *		name;
  HardwareSerial *	sr;
  uint8_t		rx_pin;
  uint8_t		tx_pin;
}			t_seriallist;

t_seriallist	gl_seriallist[] = {
  //  {"UART0", &Serial},
  {"UART1", &Serial1, 1, 0},
  {0, 0, 0, 0}
};

void	redirect_streams(Stream *src, HardwareSerial *dst, bool echo)
{
  char	c;

  //dst->begin(dst_speed);
  src->println("Starting Serial text redirection, to end it, type <Ctrl+D>");
  while (true)
    {
      if (src->available())
	{
	  c = src->read();
	  if (c == 0x04)
	    break ;
	  if (dst->write(c) && echo)
	    src->write(c);
	}
      if (dst->available())
	{
	  c = dst->read();
	  src->write(c);
	}
    }
  //dst->end();
  src->println("Stopping redirection");
}

bool		serial_begin(Stream *s, char **args)
{
  String	speed;

  if (args[2] == 0 || (args[2] && args[3] == 0))
    {
      s->println("Usage: serial begin <UARTX> <speed>");
      return (true);
    }
  speed = args[3];
  if (speed.toInt() == 0)
    {
      s->println("Error: invalid speed");
      return (false);
    }
  for (int i = 0; gl_seriallist[i].name; i++)
    if (strcmp(args[2], gl_seriallist[i].name) == 0)
      {
	if (s == gl_seriallist[i].sr)
	  {
	    s->println("Cannot begin own serial (already begun ... or do we need to follow the white rabbit ?)");
	    return (false);
	  }
	gl_seriallist[i].sr->begin(speed.toInt(), SERIAL_8N1, gl_seriallist[i].rx_pin, gl_seriallist[i].tx_pin);
	return (true);
      }
  s->println("Error: Serial name not found");
  return (false);
}

bool		serial_connect(Stream *s, char **args)
{
  bool		echo;

  if (args[2] == 0)
    {
      s->println("Usage: serial connect <UARTX> [echo]");
      s->println("echo = echo typed caracter (if remote does not)");
      return (true);
    }
  echo = ((args[3] && strcmp(args[3], "echo") == 0) ? true : false);
  for (int i = 0; gl_seriallist[i].name; i++)
    if (strcmp(args[2], gl_seriallist[i].name) == 0)
      {
	if (s == gl_seriallist[i].sr)
	  {
	    s->println("Cannot redirect to own serial (inception ?)");
	    return (false);
	  }
	redirect_streams(s, gl_seriallist[i].sr, echo);
	return (true);
      }
  s->println("Error: Serial name not found");
  return (false);
  
  return (true);
}

bool		serial_write(Stream *s, char **args)
{
  int		j;
  int		k;
  int		len;
  unsigned long	b;
  uint8_t	c;
  unsigned long towait;
  unsigned long time;

  if (args[2] == 0 || (args[2] && args[3] == 0) || (args[3] && args[4] == 0))
    {
      s->println("Usage: serial write <UARTX> <read_timeout_ms> <Hex_Byte01>[,HexByte02,HexByte03,HexByte04...]");
      s->println("UARTX = the serial to use");
      s->println("read_timeout_ms = timeout read waiting for answer after byte write");
      return (true);
    }
  towait = strtoul(args[3],0, 10);
  for (j = 0; gl_seriallist[j].name; j++)
    if (strcmp(args[2], gl_seriallist[j].name) == 0)
      {
	if (s == gl_seriallist[j].sr)
	  {
	    s->println("Cannot redirect to own serial (inception ?)");
	    return (false);
	  }
	for (k = 0, len = strlen(args[4]); k < len; k++)
	  if (args[4][k] == ',')
	    args[4][k] = 0;
	b = strtoul(args[4], 0, 16);
	if (b > 255)
	  {
	    s->println("Error: Not a valid Hex byte value");
	    return (false);
	  }
	c = ((uint8_t)b);
	gl_seriallist[j].sr->write(c);
	s->println("wrote 1 byte");
	for (k = 0; k < len; k++)
	  if (k && args[4][k] == 0 && (k + 1) < len && args[4][k + 1] != 0)
	    {
	      b = strtoul((args[4] + k + 1), 0, 16);
	      if (b > 255)
		{
		  s->println("Error: Not a valid Hex byte value");
		  return (false);
		}
	      c = ((uint8_t)b);
	      gl_seriallist[j].sr->write(c);
	      s->println("wrote another byte");
	    }
	time = millis();
	while ((millis() - time) < towait)
	  if (gl_seriallist[j].sr->available())
	    {
	      s->print(gl_seriallist[j].sr->read(), HEX);
	      s->print(" ");
	      time = millis();
	    }
	  else
	    delay(10);
	s->println();
	return (true);
      }
  s->println("Error: Serial name not found");
  return (false);
  
  return (true);
}

bool		serial_end(Stream *s, char **args)
{

  if (args[2] == 0)
    {
      s->println("Usage: serial end <UARTX>");
      return (true);
    }
  for (int i = 0; gl_seriallist[i].name; i++)
    if (strcmp(args[2], gl_seriallist[i].name) == 0)
      {
	if (s == gl_seriallist[i].sr)
	  {
	    s->println("Cannot end own serial (seppuku ?)");
	    return (false);
	  }
	gl_seriallist[i].sr->end();
	return (true);
      }
  s->println("Error: Serial name not found");
  return (false);
}


t_genericcmd	gl_serialcmd[] = {
  {"begin", serial_begin},
  {"connect", serial_connect},
  {"write", serial_write},
  {"end", serial_end},
  {0, 0}
};

bool		serial(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      for (int i = 0; gl_seriallist[i].name; i++)
	if (s == gl_seriallist[i].sr)
	  {
	    s->print("Actual serial is ");
	    s->println(gl_seriallist[i].name);
	  }
      s->println("\nUsage: serial < begin <UARTX> <speed> | connect <UARTX> [echo] | write <UARTX> <timeout_ms> [HexByte01,02...] | end <UARTX> >");
      return (true);
    }
  for (int i = 0; gl_serialcmd[i].name; i++)
    if (strcmp(args[1], gl_serialcmd[i].name) == 0)
      {
	gl_serialcmd[i].fct(s, args);
	return (true);
      }
  return (false);
}

bool		exec(espShell *sh, Stream *s, char **args)
{
  String        line;
  File          fd;
  char          *newline;
  unsigned int  newlinelen = 16;
  unsigned int  newendline = 0;

  line = "";
  if (args[1] == 0)
    {
      s->println("Usage: exec <cmdfile>");
      return (false);
    }
  if (args[1][0] == '/')
    line = args[1];
  else
    {
      line = sh->path();
      line += args[1];
    }
  fd = SPIFFS.open(line.c_str(), "r");
  if (!fd)
    {
      s->print("Could not open file: ");
      s->println(line.c_str());
      return (false);
    }
  newline = (char *)(xmalloc(sizeof(*newline) * (newlinelen + 1)));
  memset(newline, 0, newlinelen);
  while (fd.available())
    {
      newline[newendline] = fd.read();
      if (newendline == 0 && newline[newendline] == '\n')
	continue;
      if (sh->interactive())
	s->print(newline[newendline]);
      if (newline[newendline] == '\n')
        {
	  newline[newendline] = 0;
	  if (newendline > 0 && newline[newendline - 1] == '\r')
	    newline[newendline - 1] == 0;
	  sh->runLine(newline);
          free(newline);
          newlinelen = 16;
          newendline = 0;
          newline = (char *)(xmalloc(sizeof(*newline) * (newlinelen + 1)));
          memset(newline, 0, newlinelen);
          continue;
        }
      newendline++;
      if (newendline >= newlinelen)
        {
	  newlinelen *= 2;
          char *tmp = (char *)(xmalloc(sizeof(*tmp) * (newlinelen + 1)));
          memset(tmp, 0, newlinelen);
          memcpy(tmp, newline, sizeof(*newline) * (newendline));
          free(newline);
          newline = tmp;
        }
    }
  fd.close();
  return (true);
}


bool	cmd_delay(espShell *sh, Stream *s, char **args)
{
  String time;
  if (args[1] == 0)
    {
      s->println("Usage: delay <ms>");
      return (false);
    }
  time = args[1];
  delay(time.toInt());
  return (true);
}


bool	sleep_status(Stream *s, char **args)
{
  if (CanCarCfg.getValue("EnableSleep") == "true")
    s->println("Sleep is enabled");
  else
    s->println("Sleep is disabled");
  if (CanCarCfg.getValue("InactivityTimeout") != "")
    {
      s->print("Will go to Sleep after ");
      s->print(CanCarCfg.getValue("InactivityTimeout"));
      s->println(" seconds of Inactivity");
    }
  else
    {
      if (CanCarCfg.getValue("EnableSleep") == "true")
	s->println("BUT will not go to Sleep because no InactivityTimeout is set");
      else
	s->println("And not InactivityTimeout is set");
    }
  return (true);
}

bool	sleep_disable(Stream *s, char **args)
{
  CanCarCfg.setValue("EnableSleep", "false");
  s->println("Sleep is now disabled");
  return (true);
}

bool	sleep_enable(Stream *s, char **args)
{
  CanCarCfg.setValue("EnableSleep", "true");
  s->println("Sleep is now enabled");
  return (true);
}

bool	sleep_time(Stream *s, char **args)
{
  if (args[2] == 0)
    {
      if (CanCarCfg.getValue("InactivityTimeout") == "")
	s->println("InactivityTimeout is not set yet");
      else
	{
	  s->print("InactivityTimeout is actually set to ");
	  s->print(CanCarCfg.getValue("InactivityTimeout"));
	  s->println(" seconds");
	}
      return (true);
    }
  String time;
  unsigned int t;
  String res;

  time = args[2];
  t = time.toInt();
  res = t;
  CanCarCfg.setValue("InactivityTimeout", res.c_str());
  s->print("InactivityTimeout is now set to ");
  s->print(res);
  s->println(" seconds");
  return (true);
}

bool	sleep_now(Stream *s, char **args)
{
  CanCarControl.goToSleep();
  return (true); // should never see this line ... but ... needs to be function pointing table compliant ^^" 
}
t_genericcmd	gl_sleepcmd[] = {
  {"status", sleep_status},
  {"disable", sleep_disable},
  {"enable", sleep_enable},
  {"time", sleep_time},
  {"now", sleep_now},
  {0, 0}
};

bool	cmd_sleep(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: sleep <status|disable|enable|time [value]>");
      return (false);
    }
  for (int i = 0; gl_sleepcmd[i].name; i++)
    if (strcmp(args[1], gl_sleepcmd[i].name) == 0)
      {
	gl_sleepcmd[i].fct(s, args);
	return (true);
      }
  return (false);
}

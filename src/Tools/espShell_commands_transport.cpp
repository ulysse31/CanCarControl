#include "CanCarControl.h"

bool		xmreceive(espShell *sh, Stream *s, char **args)
{
  String	line = "";
  File		fd;
  uint8_t	c;
  unsigned long filesize;
  unsigned int 	read = 0;
  
  if (args[1] == 0 || (args[1] != 0 && args[2] == 0))
    {
      s->println("Usage: xmreceive <file> <size>");
      return (false);
    }
  if (args[1][0] == '/')
    line = args[1];
  else
    {
      line = sh->path();
      line += args[1];
    }
  filesize = strtoul(args[2], NULL, 10);
  fd = SPIFFS.open(line.c_str(), "w");
  if (!fd)
    {
      s->print("Could not open file: ");
      s->println(line.c_str());
      return (false);
    }
  while(s->available())
    c = s->read();
  s->println("DO NOT TYPE ANYTHING MANUALLY FROM HERE ON");
  s->println("Starting data storage into file " + line + " expecting size " + filesize);
  s->println("You can now send data via XMODEM protocol");
  XmodemReceiver Xrecv;
  Xrecv.init(s, &fd, filesize);
  int ret = XmodemReceiver::RUNNING;
  do {
    ret = Xrecv.run();
    // FIXME: XModem receiver runs in a busy loop and for some reason
    // under some conditions doesn't allow any other even higher priority
    // threads to be scheduled. Most likely this is some kind of an issue
    // with priority inheritance. As a temporary workaround, we'll just
    // add a 1 tick delay here which should guarantee that other threads
    // get CPU time.
    vTaskDelay(1 / portTICK_PERIOD_MS);
  } while (ret == XmodemReceiver::RUNNING);
  Xrecv.destroy();
  fd.close();
  s->println("File wrote.");
  return (true);
}
/*
#define HEXLINE_BUFFSIZE	4

bool		hexrecv(espShell *sh, Stream *s, char **args)
{
  String	line = "";
  File		fd;
  char		buff[HEXLINE_BUFFSIZE];
  unsigned int	toread = 0;
  unsigned int	totalwrite = 0;
  bool		arm = false;
  unsigned long	lasttime;
  
  if (args[1] == 0)
    {
      s->println("Usage: hexrecv <file>");
      return (false);
    }
  if (args[1][0] == '/')
    line = args[1];
  else
    {
      line = sh->path();
      line += args[1];
    }
  fd = SPIFFS.open(line.c_str(), "w");
  if (!fd)
    {
      s->print("Could not open file: ");
      s->println(line.c_str());
      return (false);
    }
  memset(buff, 0, HEXLINE_BUFFSIZE * sizeof(*buff));
  s->println("Starting data storage into file " + line);
  s->println("You can now send data with correct format with command `od -A x -v -t x1 <file> > /dev/ttyname`");
  s->println("Once ended, type <Ctrl+D> or Esc to close");
  lasttime = millis();
  while (true)
    {
      while((toread = s->available()) == 0)
	;
      if ((buff[0] = s->read()) == ' ')
	arm = true;
      if (buff[0] == 0x04 || buff[0] == 0x1B)
	break ;
      if ((millis() - lasttime) > 2000) // around every 2 secs
	{
	  lasttime = millis();
	  s->print("\r");
	  s->print(toread);
	  s->print(" - ");
	  s->print(totalwrite);
	}
      if (arm)
	{
	  while(s->available() == 0)
	    ;
	  buff[0] = s->read();
	  while(s->available() == 0)
	    ;
	  buff[1] = s->read();
	  long l;
	  uint8_t e;
	  l = strtol(buff, NULL, 16);
	  e = ((uint8_t)l);
	  fd.write(e);
	  arm = false;
	  buff[0] = 0;
	  buff[1] = 0;
	  totalwrite++;
	}
    }
  s->print("\nTotal written bytes: ");
  s->println(totalwrite);
  fd.close();
  return (true);
}
*/
bool		fwupdate(espShell *sh, Stream *s, char **args)
{
  String	line = "";
  File		fd;

  if (args[1] == 0)
    {
      s->println("Usage: fwupdate <file>");
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
  size_t fileSize = fd.size();
  if ((!fd) || !Update.begin(fileSize))
    {
      s->println("Update Error: Firmware file seems invalid");
      if (fd)
	fd.close();
      return (false);
    }
  Update.writeStream(fd);
  if(Update.end())
    {
      s->println("Update Successful");
      fd.close();
      ESP.restart();
    }
  else
    {
      s->println("Update Error: " + String(Update.getError()));
      fd.close();
      return (false);
    }
  fd.close();
  return (true);
}

bool		restart(espShell *sh, Stream *s, char **args)
{
   ESP.restart();
   return (true);
}

bool		interactive(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: interactive on|off");
      return (false);
    }
  if (strcmp(args[1], "on") == 0)
    sh->interactive(true);
  if (strcmp(args[1], "off") == 0)
    sh->interactive(false);
  return (true);
}

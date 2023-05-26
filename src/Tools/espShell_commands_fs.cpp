#include "CanGlobal.h"

bool		cat(espShell *sh, Stream *s, char **args)
{
  String	line;
  File		fd;

  line = "";
  if (args[1] == 0)
    {
      s->println("Usage: cat <file>");
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
  while (fd.available())
      s->write(fd.read());
  fd.close();
  return (true);
}

const char * getfullpath(const char *ptr)
{
  while (*ptr != 0)
    ptr--;
  return (ptr + 1);
}

bool	ls(espShell *sh, Stream *s, char **args)
{
  File root = SPIFFS.open(sh->path());
  File file = root.openNextFile();

  s->print("Directory Listing: ");
  s->println(sh->path());
  s->println("");
  s->println("\t FILE:\t\tSIZE:");
  while(file)
    {
      s->print("\t");
      s->print(getfullpath(file.name()));
      s->print("\t");
      s->println(file.size());
      file.close();
      file = root.openNextFile();
    }
  root.close();
  //listDir(SPIFFS, "/", 0);
  return (true);
}

bool		md5sum(espShell *sh, Stream *s, char **args)
{
  String        line = "";
  File		fd;
  MD5Builder	md5;

  if (args[1] == 0)
    {
      s->println("Usage: md5sum <file>");
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
  md5.begin();
  md5.addStream(fd, fd.size());
  md5.calculate();
  s->print(md5.toString());
  s->print('\t');
  s->println(line.c_str());
  fd.close();
  return (true);
}

bool    rm(espShell *sh, Stream *s, char **args)
{
  String	line;
  File		fd;
  unsigned int	k;

  line = "";
  if (args[1] == 0)
    {
      s->println("Usage: rm <file1> <file2> ... ");
      s->println("or");
      s->println("Usage: rm -all\nTo remove all non-config files (non '^/cfg/' files)");
      return (false);
    }
  if (strcmp(args[1], "-all") == 0)
    {
      s->println("Deleting all non-config files...");
      File root = SPIFFS.open(sh->path());
      File file = root.openNextFile();
      while(file)
	{
	  if (strncmp("/cfg/", getfullpath(file.name()), strlen("/cfg/")) != 0)
	    SPIFFS.remove(getfullpath(file.name()));
	  file.close();
	  file = root.openNextFile(); 
	}
      root.close();
      return (true);
    }
  for (k = 1; args[k]; k++)
    {
      if (args[k][0] == '/')
	line = args[k];
      else
	{
	  line = sh->path();
	  line += args[k];
	}
      if (SPIFFS.remove(line.c_str()) == false)
	{
	  s->print("Could not delete ");
	  s->println(line.c_str());
	}
    }
  return (true);
}

bool		cp(espShell *sh, Stream *s, char **args)
{
  String	line;
  String	line2;
  File		fd;
  File		fd2;

  line = "";
  line2 = "";
  if (args[1] == 0 || (args[1] != 0 && args[2] == 0))
    {
      s->println("Usage: cp <sourcefile> <destinationfile>");
      return (false);
    }
  if (args[1][0] == '/')
    line = args[1];
  else
    {
      line = sh->path();
      line += args[1];
    }
  if (args[2][0] == '/')
    line2 = args[2];
  else
    {
      line2 = sh->path();
      line2 += args[2];
    }
  fd = SPIFFS.open(line.c_str(), "r");
  if (!fd)
    {
      s->print("Could not open file: ");
      s->println(line.c_str());
      return (false);
    }
  if (SPIFFS.exists(line2.c_str()))
    SPIFFS.remove(line2.c_str());
  fd2 = SPIFFS.open(line2.c_str(), "w");
  if (!fd2)
    {
      s->print("Could not open file: ");
      s->println(line2.c_str());
      if (fd)
      fd.close();
      return (false);
    }
  while (fd.available())
      fd2.write(fd.read());
  fd.close();
  fd2.close();
  return (true);
}

bool    mv(espShell *sh, Stream *s, char **args)
{
  String	line;
  String	line2;

  line = "";
  line2 = "";
  if (args[1] == 0 || (args[1] != 0 && args[2] == 0))
    {
      s->println("Usage: mv <oldfilename> <newfilename>");
      return (false);
    }
  if (args[1][0] == '/')
    line = args[1];
  else
    {
      line = sh->path();
      line += args[1];
    }
    if (args[2][0] == '/')
    line2 = args[2];
  else
    {
      line2 = sh->path();
      line2 += args[2];
    }
    if (SPIFFS.rename(line.c_str(), line2.c_str()) == false)
      {
	s->print("Could not rename ");
	s->print(line.c_str());
	s->print(" into ");
	s->println(line2.c_str());
	return (false);
      }
  return (true);
}

// if no caracters are typed in during 5 mins ... well ...
// lets end up the job in order to avoid being stuck ...
#define EDITION_TIMEOUT		3*60*1000

bool    ed(espShell *sh, Stream *s, char **args)
{
  String	line;
  File		fd;
  char		*newline;
  unsigned int	newlinelen = 16;
  unsigned int	newendline = 0;
  bool		interactive;
  unsigned long time;

  line = "";
  if (args[1] == 0)
    {
      s->println("Usage: ed <file>");
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
  interactive = sh->interactive();
  if (interactive)
    {
      s->print("Adding Edited lines inside ");
      s->println(line.c_str());
      s->println("Once ended, type <Ctrl+D>");
      s->println("Process will end after a period of inactivity of 5 mins");
    }
  newline = (char *)(xmalloc(sizeof(*newline) * (newlinelen + 1)));
  memset(newline, 0, newlinelen);
  time = millis();
  while (true)
    {
      while(s->available() == 0 && ((millis() - time) < EDITION_TIMEOUT))
	delay(10);
      if ((millis() - time) > EDITION_TIMEOUT) // mo genkai desu ... akiramero 
	{
	  if (newendline)
	    {
	      newline[newendline] = 0x00;
	      fd.write((uint8_t *)newline, newendline+1);
	    }
	  free(newline);
	  newlinelen = 0;
	  newendline = 0;
	  if (interactive)
	    s->println("");
	  break;
	}
      newline[newendline] = s->read();
      time = millis();
      if (newendline == 0 && newline[newendline] == '\n')
        continue;
      if (newline[newendline] == 0x04 || newline[newendline] == 0x1B)
	{
	  if (newendline)
	    {
	      newline[newendline] = 0x00;
	      fd.write((uint8_t *)newline, newendline+1);
	    }
	  free(newline);
	  newlinelen = 0;
	  newendline = 0;
	  if (interactive)
	    s->println("");
	  break;
	}
      if (newendline == 0 && newline[newendline] == 0x08)
        {
          newline[newendline] = 0;
          continue;
        }
      if (newline[newendline] == 0x08 && newendline > 0)
        {
	  if (interactive)
	    {
	      s->print('\b');
	      s->print(' ');
	      s->print('\b');
	    }
          newline[newendline] = 0;
          newendline = newendline - 1;
          continue;
        }
      if (interactive)
	s->print(newline[newendline]);
      if (newline[newendline] == '\n')
        {
	  fd.write((uint8_t *)newline, newendline+1);
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

#include "CanGlobal.h"

typedef struct	s_cfgcmd
{
  const char *	name;
  bool		(*fct)(Stream *s, char **args);
}		t_cfgcmd;


bool		cfg_list(Stream *s, char **args)
{
  cfgNode	*p;

  s->println("KEY:\tVALUE:");
  for (p = CanCfg.startNode(); p; p = p->next())
    {
      s->print(p->key());
      s->print(":\t");
      s->println(p->value());
    }
  return (true);
}

bool		cfg_get(Stream *s, char **args)
{
  String	res;

  if (args[2] == 0)
    {
      s->println("Usage: cfg get <key>");
      return (false);
    }
  res =CanCfg.getValue(args[2]); 
  if (res == "")
    {
      s->print(args[2]);
      s->println(": key not found");
      return (false);
    }
  else
    {
      s->print(args[2]);
      s->print(":\t");
      s->println(res);
    }
  return (true);
}

bool		cfg_set(Stream *s, char **args)
{
  String	key;
  String	val;

  if (args[2] == 0 || (args[2] && args[3] == 0))
    {
      s->println("Usage: cfg set <key> <value>");
      return (false);
    }
  key = args[2];
  val = args[3];
  if (key == "")
    {
      s->println("Error: Key cannot be empty");
      return (false);
    }
  if (val == "")
    {
      s->println("Error: Value cannot be empty");
      return (false);
    }
  if(CanCfg.setValue(key, val) == false)
    {
      s->println("Error: Could not set key/value pair");
      return (false);
    }
  return (true);
}

bool	cfg_del(Stream *s, char **args)
{
  if (args[2] == 0)
    {
      s->println("Usage: cfg del <key>");
      return (false);
    }
  if(CanCfg.delValue(args[2]) == false)
    {
      s->println("Error: Could not delete key => not found?");
      return (false);
    }
  return (true);
}

bool	cfg_save(Stream *s, char **args)
{
  if (CanCfg.saveCfg() == false)
    {
      s->println("Error: Could not save config");
      return (false);
    }
  s->println("Configuration successfuly saved");
  return (true);
}

t_cfgcmd	gl_cfgcmd[] =
  {
    {"list", cfg_list},
    {"get", cfg_get},
    {"set", cfg_set},
    {"del", cfg_del},
    {"save", cfg_save},
    {0, 0}
  };


bool	cfg(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: cfg [list|save|get <key>|set <key> <value>|del <key>]");
      return (false);
    }
  for (int i = 0; gl_cfgcmd[i].name; i++)
    if (strcmp(args[1], gl_cfgcmd[i].name) == 0)
      return(gl_cfgcmd[i].fct(s, args));
  s->println("Usage: cfg [list|get <key>|set <key> <value>|del <key>]");
  return (false);
}


#include "CanGlobal.h"

typedef struct	s_aliascmd
{
  const char *	name;
  bool		(*fct)(Stream *s, char **args);
}		t_aliascmd;


bool		alias_list(Stream *s, char **args)
{
  cfgNode	*p;

  s->println("Alias:\tCommand:");
  for (p = Aliases.startNode(); p; p = p->next())
    {
      s->print(p->key());
      s->print(":\t");
      s->print('"');
      s->print(p->value());
      s->println('"');
    }
  return (true);
}

bool		alias_get(Stream *s, char **args)
{
  String	res;

  if (args[2] == 0)
    {
      s->println("Usage: alias get <alias>");
      return (false);
    }
  res =Aliases.getValue(args[2]); 
  if (res == "")
    {
      s->print(args[2]);
      s->println(": alias not found");
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

bool		alias_set(Stream *s, char **args)
{
  String	key;
  String	val;

  if (args[2] == 0 || (args[2] && args[3] == 0))
    {
      s->println("Usage: alias set <alias> <cmd>");
      return (false);
    }
  key = args[2];
  val = args[3];
  if (key == "")
    {
      s->println("Error: alias cannot be empty");
      return (false);
    }
  if (val == "")
    {
      s->println("Error: command cannot be empty");
      return (false);
    }
  if(Aliases.setValue(key, val) == false)
    {
      s->println("Error: Could not set alias/command pair");
      return (false);
    }
  return (true);
}

bool	alias_del(Stream *s, char **args)
{
  if (args[2] == 0)
    {
      s->println("Usage: alias del <alias>");
      return (false);
    }
  if(Aliases.delValue(args[2]) == false)
    {
      s->println("Error: Could not delete alias => not found?");
      return (false);
    }
  return (true);
}

bool	alias_save(Stream *s, char **args)
{
  if (Aliases.saveCfg() == false)
    {
      s->println("Error: Could not save alias config");
      return (false);
    }
  s->println("Alias Configuration successfuly saved");
  return (true);
}

t_aliascmd	gl_aliascmd[] =
  {
    {"list", alias_list},
    {"get", alias_get},
    {"set", alias_set},
    {"del", alias_del},
    {"save", alias_save},
    {0, 0}
  };


bool	alias(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: alias [list|save|get <alias>|set <alias> <cmd>|del <alias>]");
      return (false);
    }
  for (int i = 0; gl_aliascmd[i].name; i++)
    if (strcmp(args[1], gl_aliascmd[i].name) == 0)
      return(gl_aliascmd[i].fct(s, args));
  s->println("Usage: alias [list|get <alias>|set <alias> <cmd>|del <alias>]");
  return (false);
}


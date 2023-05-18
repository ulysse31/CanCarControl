#ifndef __ESPMoticsCfg_H__
# define __ESPMoticsCfg_H__

# include <SPIFFS.h>
# define BASEPATH "/etc/"
# define BUF_SIZE     32


class cfgNode
{
  public:
  cfgNode() { _key = ""; _val = ""; _next = 0; }
  cfgNode(String *key, String *val, cfgNode *next) { _key = *key; _val = *val; _next = next; }
  ~cfgNode() { }
  String 	key(String *k = 0) { if (k) _key = *k; return (_key); }
  const char * 	ckey(String *k = 0) { if (k) _key = *k; return (_key.c_str()); }
  String	value(String *v = 0) { if (v) _val = *v; return (_val); }
  const char *	cvalue(String *v = 0) { if (v) _val = *v; return (_val.c_str()); }
  cfgNode *	next(cfgNode *node = 0) { if (node) _next = node; return (_next); }
 private:
  String	_key;
  String	_val;
  cfgNode *	_next;
};

class ESPMoticsCfg
{
 public:
  ESPMoticsCfg(const char *name, char separator);
  ESPMoticsCfg(String name, char separator);
  ~ESPMoticsCfg();
  bool		loadCfg();
  bool		saveCfg();
  String	getValue(const char *key) { String k = key; return(getValue(k)); }
  String	getValue(String key);
  const char *	getcValue(String key);
  bool		setValue(const char *key, const char *value) { String k = key; String v = value; return(setValue(k, v)); }
  bool		setValue(String key, String value);
  bool		delValue(String key);
  bool		delValue(const char *key) { String k = key; return(delValue(k)); }
  char		separator(char c = 0) { if (c != 0) _separator = c; return (_separator); }
  cfgNode *	startNode() { return(_node); }

private:
  char		_separator;
  File		_fd;
  String	_filename;
  String	_filepath;
  cfgNode *	_node;
};
#endif // !__ESPMoticsCfg_H__ //

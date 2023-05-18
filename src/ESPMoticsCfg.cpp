#include "ESPMoticsCfg.h"

//
// ESPMoticsCfg Object Methods
//

ESPMoticsCfg::ESPMoticsCfg(const char *name, char separator = '=')
{
  _filename = name;
  _separator = separator;
  _filepath = String(BASEPATH) + _filename; 
  _node = 0;
}

ESPMoticsCfg::ESPMoticsCfg(String name, char separator = '=')
{
  _filename = name;
  _separator = separator;
  _filepath = String(BASEPATH) + _filename; 
  _node = 0;
}

ESPMoticsCfg::~ESPMoticsCfg()
{

}

bool
ESPMoticsCfg::loadCfg()
{
  String  line;

  //  Serial.print("DBG: Opening cfg file ");
  //  Serial.println(_filepath);
  _fd = SPIFFS.open(_filepath, "r");
  if (!_fd)
    return (false);
  while (_fd.available())
  {
    line = _fd.readStringUntil('\n');
    if (_node)
      {
        cfgNode *n;
        String k = line.substring(0, line.indexOf(_separator));
        String v = line.substring(line.indexOf(_separator) + 1);
        n = new cfgNode(&(k), &(v), _node);
        _node = n;
      }
      else
      {
        String k = line.substring(0, line.indexOf(_separator));
        String v = line.substring(line.indexOf(_separator) + 1);
        _node = new cfgNode(&(k), &(v), 0);
      }
  }
  _fd.close();
  return (true);
}

bool
ESPMoticsCfg::saveCfg()
{
  String  line;
  cfgNode *p;
  String res;

  _fd = SPIFFS.open(_filepath, "w");
  if (!_fd)
    return (false);
  for (p = _node; p; p = p->next())
    {
      line = p->key();
      line += '=';
      line += p->value();
      line += '\n';
      _fd.print(line);
    }
  _fd.close();
  return (true);
}

String
ESPMoticsCfg::getValue(String key)
{
  cfgNode *p;
  String res = "";

  for (p = _node; p; p = p->next())
    if (p->key() == key)
      return (p->value());
  return (res);
}

const char *
ESPMoticsCfg::getcValue(String key)
{
  cfgNode *p;

  for (p = _node; p; p = p->next())
    if (p->key() == key)
      return (p->cvalue());
  return (0);
}

bool
ESPMoticsCfg::setValue(String key, String value)
{
  cfgNode *p;
  String res;

  for (p = _node; p; p = p->next())
    if (p->key() == key)
      {
        p->value(&value);
        return (true);
      }
  cfgNode *n;
  n = new cfgNode(&key, &value, _node);
  _node = n;
  return (true);
}

bool
ESPMoticsCfg::delValue(String key)
{
  cfgNode *p;
  cfgNode *last;
  String res;

  for (p = _node, last = 0; p; last = p, p = p->next())
    if (p->key() == key)
      {
	if (last == 0)
	  {
	    _node = p->next();
	    delete (p);
	    return (true);
	  }
	else
	  {
	    last->next(p->next());
	    delete (p);
	    return (true);
	  }
      }
  return (false);
}

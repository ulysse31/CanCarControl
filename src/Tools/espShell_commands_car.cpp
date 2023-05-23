#include "CanCarControl.h"

// this file is car specific
// you might want to add here your own functions that need to process data from car


typedef union	u_gpspos
{
  uint8_t	byte[4];
  unsigned int	pos;
}		t_gpspos;

typedef struct	s_gps
{
  t_gpspos	longitude;
  t_gpspos	latitude;
}		t_gps;

bool		gps(espShell *sh, Stream *s, char **args)
{
  t_gps		g;
  INT32U        id;
  uint8_t       len;
  uint8_t       data[8];
  unsigned long init = 0;
  unsigned long timeout = 1000 * 60 * 3; // 3 min timeout for now
  double flong;
  double flat;

  init = millis();
  while (true)
    {
      if (mcp2515.readMsgBuf(&id, &len, data) == CAN_OK && id == 0x34A)
	{
	  s->print(millis());
	  s->print(",0x");
	  s->print(id, HEX);
	  s->print(",0x");
	  s->print(len, HEX);
	  for (int i = 0; i < len; i++)
	    {
	      s->print(",0x");
	      s->print(data[i],HEX);
	      if (i < 4)
		g.longitude.byte[i] = data[i];
	      else
		g.latitude.byte[i % 4] = data[i];
	    }
	  s->println();
	  flong = ((((double)g.longitude.pos) / 11930464.71) - 360);
	  flat =  ((((double)g.latitude.pos) / 11930464.71) - 360);
	  s->println();
	  s->println(flong);
	  s->println(flat);
	  return (true);
	}
      else
	if ((millis() - init) > timeout)
	  return (false);
    }
  return (true);
}

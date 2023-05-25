#include "CanCarControl.h"

// this file is car specific
// you might want to add here your own functions that need to process data from the car


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

typedef struct	s_doorstatus
{
  uint8_t	open_status;
  uint8_t	lock_status;
  uint8_t	filled;
}		t_doorstatus;

# define FL_DOOR	0
# define FR_DOOR	1
# define RL_DOOR	2
# define RR_DOOR	3
# define BOOT_DOOR	4

# define NB_DOORS	5

# define FL_CANID	0xEA
# define FR_CANID	0xE2
# define RL_CANID	0xEE
# define RR_CANID	0xE6
# define BOOT_CANID	0xF2


# define DOOR_UNLOCKED	0x81
# define DOOR_CLOSED	0xFC
# define BOOT_UNLOCKED	0xF1
# define BOOT_CLOSED	0xC0

typedef	struct	s_doortable
{
  uint8_t	i;
  uint16_t	canid;
  const char *	str;
}		t_doortable;

t_doortable	gl_doortable[] = {
  {FL_DOOR, FL_CANID, "Front Left:"},
  {FR_DOOR, FR_CANID, "Front Right:"},
  {RL_DOOR, RL_CANID, "Rear Left:"},
  {RR_DOOR, RR_CANID, "Rear Right:"},
  {BOOT_DOOR, BOOT_CANID, "Boot:"},
  {0, 0, 0}
};

bool		doorstatus(espShell *sh, Stream *s, char **args)
{
  INT32U        id;
  uint8_t       len;
  uint8_t       data[8];
  unsigned long init = 0;
  unsigned long timeout = 1000 * 60; // 1 min timeout for now
  t_doorstatus	doorstat[NB_DOORS];
  uint8_t	count;
  
  init = millis();
  memset(doorstat, 0, sizeof(*doorstat) * NB_DOORS);
  count = 0;
  while (count < NB_DOORS)
    {
      if (mcp2515.readMsgBuf(&id, &len, data) == CAN_OK &&
	  (id == FL_CANID || id == FR_CANID || id == RL_CANID
	   || id == RR_CANID || id == BOOT_CANID))
	{
	  for (int i = 0; gl_doortable[i].canid; i++)
	    if (id == gl_doortable[i].canid && doorstat[gl_doortable[i].i].filled == 0)
	      {
		doorstat[gl_doortable[i].i].lock_status = data[0];
		doorstat[gl_doortable[i].i].open_status = data[3];
		doorstat[gl_doortable[i].i].filled = 1;
		init = millis();
		count++;
		break ;
	      }
	}
      else
	if ((millis() - init) > timeout)
	  {
	    s->println("Timeout getting all values...");
	    break ;
	  }
    }
  for (int i = 0; gl_doortable[i].canid; i++)
    if (doorstat[gl_doortable[i].i].filled)
      {
	s->println(gl_doortable[i].str);
	s->print("-");
	s->print((doorstat[gl_doortable[i].i].lock_status == DOOR_UNLOCKED || doorstat[gl_doortable[i].i].lock_status == BOOT_UNLOCKED ? "Unlock," : "Lock,"));
	s->print((doorstat[gl_doortable[i].i].open_status == DOOR_CLOSED || doorstat[gl_doortable[i].i].open_status == BOOT_CLOSED ? "Close" : "Open"));
	s->println();
      }
    else
      {
	s->println(gl_doortable[i].str);
	s->println("-Timeout...");	
      }
  return (true);
  
}

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

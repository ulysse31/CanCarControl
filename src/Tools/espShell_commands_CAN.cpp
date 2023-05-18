#include "CanCarControl.h"

# define MAX_TX_ERRS 3

bool            send_packet(INT32U id, INT8U len, INT8U *data)
{
  uint8_t       timeout = 0;

  while (mcp2515.sendMsgBuf(id, len, data) != CAN_OK && timeout <= MAX_TX_ERRS)
     timeout++;
  if (timeout >= MAX_TX_ERRS)
    return (false);
  else
    return (true);
}

bool		canwrite(espShell *sh, Stream *s, char **args)
{
  INT32U	id = 0;
  uint8_t	len = 0;
  uint8_t	data[8];
  int		c;

  if (args[1] == 0 || (args[1] && args[2] == 0))
    {
      if (sh->interactive())
	{
	  s->println("Usage: canwrite <HexArbID> <length> HEX00 HEX01 HEX02...");
	  s->println("or");
	  s->println("Usage: canwrite -f <file> [loop]");
	  s->println("where file contains a list of lines of type: ^<timewait_ms>,<HexMessageID>,<HexLength>,[HexByte0],[HexByte1],[HexByte2][...][HexByte[8]]");
	  s->println("add optional loop argument to loop on file content");
	}
      return (false);
    }
  if (strcmp(args[1], "-f") == 0)
    return (canwritefile(sh, s, args + 1));
  memset(data, 0, sizeof(*data)*8);
  id = strtoul(args[1], NULL, 16);
  if ((len = strtoul(args[2], NULL, 16)) == 0 || len > 8)
    {
      s->println("Error: invalid length");
      s->println(args[2]);
      s->println(len);
      return (false);
    }
	
  for (c = 3; args[c] && c < 12; c++)
    data[c-3] = ((uint8_t)strtol(args[c], NULL, 16));
  if (sh->interactive())
    s->printf("Sending CAN MsgID: %x, length: %i, data[0]=%x, data[1]=%x, data[2]=%x, data[3]=%x, data[4]=%x, data[5]=%x, data[6]=%x, data[7]=%x\n",
	      id, len, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  else
    s->print(".");
  if (send_packet(id, len, data) == false)
    {
      s->println("Error sending packet");
      return (false);
    }
  return (true);
}

bool		write_canline(INT32U time, espShell *sh, Stream *s, char *buff)
{
  long int	todelay;
  INT32U	towait;
  char *	table[12]; // args0, 1MsgID, 1 len, 8 data max, 1 NULL byte
  int		linelen = 0;
  int		i;
  int		c;

  linelen = strlen(buff);
  memset(table, 0, sizeof(*table) * 12);
  table[0] = buff;
  for (i = 0, c = 0; i <= linelen; i++)
    {
      if (buff[i] == ',')
        {
          buff[i] = 0;
	  table[c + 1] = (buff + i + 1);
          c++;
        }
    }
  if (c > 11)
    return (false);
  towait = strtoul(buff, NULL, 10);
  if ((todelay = ((time + towait) - millis())) > 0)
    delay(todelay);
  canwrite(sh, s, table);
  return (true);
}


#define MAX_CAN_FILE_LINE	64

bool		canwritefile(espShell *sh, Stream *s, char **args)
{
  String        line = "";
  char		buff[MAX_CAN_FILE_LINE];
  uint8_t	i = 0;
  INT32U	time;
  File          fd;
  char		c;
  bool		loop = false;
  bool		valid = true;

  if (args[2] && strcmp(args[2], "loop") == 0)
    loop = true;
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
  memset(buff, 0, MAX_CAN_FILE_LINE);
  time = millis();
  while (valid)
    {
      fd.seek(0, SeekSet);
      i = 0;
      while (valid && fd.available())
	{
	  buff[i] = fd.read();
	  if (buff[i] == '\n')
	    {
	      buff[i] = 0;
	      if (i > 0 && buff[i - 1] == '\r')
		buff[i - 1] = 0;
	      if (write_canline(time, sh, s, buff) == false)
		valid = false;
	      memset(buff, 0, MAX_CAN_FILE_LINE);
	      i = 0;
	    }
	  else
	    i++;
	  if (i >= MAX_CAN_FILE_LINE)
	    valid = false;
	}
      if (s->available() && ((c = s->read()) == 0x04) || (c == 0x1B))
	loop = false;
      if (loop == false)
	break ;
      time = millis();
    }
  if (!valid)
    s->println("Error: Invalid can file");
  fd.close();
  return (valid);
}

bool		candump(espShell *sh, Stream *s, char **args)
{
  INT32U	id;
  uint8_t	len;
  uint8_t	data[8];
  char		c;
  bool		showdump = false;
  
  s->println("-------------------------------------------------");
  s->println("-                  Can Dumper                   -");
  s->println("-                Type <d> or <D>                -");
  s->println("-         to enable/disable Can Dumping         -");
  s->println("-            Type <Ctrl+D> or Esc               -");
  s->println("-            to quit this command               -");
  s->println("-------------------------------------------------");
  while (true)
    {
      while(s->available() == 0)
	if (mcp2515.readMsgBuf(&id, &len, data) == CAN_OK && showdump == true)
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
	      }
	    s->println();
	  }
      c = s->read();
      if (c == 'd' || c == 'D')
	{
	  showdump = (showdump == true ? false : true);
	  if (showdump)
	    {
	      s->println();
	      s->println("-------------------------------------------------");
	      s->println("------------------- Dump Start ------------------");
	      s->println("------------ Type <d> or <D> to stop ------------");
	      s->println("------------ <Ctrl+D> or Esc to quit ------------");
	      s->println("TIME:,ID:,DLC:,DATA:");
	    }
	  else
	    {
	      s->println();
	      s->println("------------------- Dump Stop -------------------");
	      s->println("----------- Type <d> or <D> to start ------------");
	      s->println("----------- <Ctrl+D> or Esc to quit -------------");
	      s->println("-------------------------------------------------");
	    }
	}
      if (c == 0x04 || c == 0x1B)
	break ;
    }
  return (true);
}

#define CAN2A_ID_MAX	2048

bool		loadIDs(char *buff, uint8_t *IDtable)
{
  uint8_t	i;
  uint8_t	c;
  uint8_t	len;
  INT32U	MessageID;


  for (i = 0, len = strlen(buff); i < len; i++)
    if (buff[i] == ',')
      buff[i] = 0;
  MessageID = strtoul(buff, 0, 16);
  if (MessageID >= CAN2A_ID_MAX)
    return (false);
  IDtable[MessageID] = 1;
  for (i = 0, c = 1; i < len; i++)
    if (buff[i] == 0 && (i + 1) < len && buff[i + 1] != 0)
      {
	MessageID = strtoul(buff + i + 1, 0, 16);
	if (MessageID >= CAN2A_ID_MAX)
	  return (false);
	IDtable[MessageID] = 1;
      }
  return (true);
}

bool		canwait(espShell *sh, Stream *s, char **args)
{
  INT32U	id;
  uint8_t	len;
  uint8_t	data[8];
  uint8_t	IDtable[CAN2A_ID_MAX];
  unsigned long	init = 0;
  unsigned long	timetowait = 0;
  INT32U	IDtowait = 0;  
  INT32U	printed = 0;  
  char		c;
  bool		showdump = false;
  
  if (args[1] == 0)
    {
      s->println("Usage: canwait <MessagID01,MessageID02,MessageID03...> [max_message_to_wait] [max_time_to_wait]");
      s->println("\twhere max_message_to_wait = 0 for unlimited");
      s->println("\tand max_time_to_wait = 0 for unlimited");
      s->println("\tYou can press Ctrl+D or Esc to stop process any time");
      return (false);
    }
  memset(IDtable, 0, sizeof(*IDtable) * CAN2A_ID_MAX);
  if (loadIDs(args[1], IDtable) == false)
    {
      s->println("Error: Invalid ID list");
      return (false);
    }
  if (args[2])
    {
      IDtowait = strtoul(args[2], 0, 10);
      if (args[3])
	timetowait = strtoul(args[3], 0, 10);
    }
  s->println("Starting CAN waiting process");
  s->print("Waiting for ");
  s->print((IDtowait == 0 ? "Unlimited" : String(IDtowait).c_str()));
  s->println(" ID messages");
  s->print("From ");
  for (int k = 0; k < CAN2A_ID_MAX; k++)
    if (IDtable[k])
      {
	s->print("0x");
	s->print(k, HEX);
	s->print(", ");
      }
  s->println("ID identifier list");
  s->print("During ");
  s->print((timetowait == 0 ? "Unlimited" : String(timetowait).c_str()));
  s->println(" milliseconds");
  s->println("Press Ctrl+D or Esc any time to interrupt");
  init = millis();
  while (true)
    {
      while (s->available() == 0)
	if (mcp2515.readMsgBuf(&id, &len, data) == CAN_OK && IDtable[id])
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
	      }
	    s->println();
	    printed++;
	  }
	else
	  {
	    if (IDtowait && printed >= IDtowait)
	      return (true);
	    if (timetowait && (millis() - init) > timetowait)
	      return (true);
	  }
      c = s->read();
      if (c == 0x04 || c == 0x1B)
	break ;
    }
  return (true);
}

#include "CanGlobal.h"


bool    lorasecure(espShell *sh, Stream *s, char **args)
{
  if (args[1] == 0)
    {
      s->println("Usage: lorasecure on|off");
      return (false);
    }
  if (strcmp(args[1], "on") == 0)
    shellLoRa->secure(true);
  if (strcmp(args[1], "off") == 0)
    shellLoRa->secure(false);
  return (true);
}

void	showbuffhex(Stream *s, const char *title, const char *buff)
{
  s->print(title);
  for (int i = 0; i < AUTH_TOKEN_SIZE; i++)
    {
      s->print(buff[i], HEX);
      s->print(" ");
    }
  s->println();
}
void	showbuff(Stream *s, const char *title, const char *buff)
{
  s->print(title);
  for (int i = 0; i < AUTH_TOKEN_SIZE; i++)
    s->write(buff[i]);
  s->println();
}


void	auth_testings(espShell *sh, Stream *s)
{
  authToken	car;
  authToken	remote;

  if (CanCfg.getValue("LoraRemoteKey") == "" || CanCfg.getValue("LoraCarKey") == ""
      || CanCfg.getValue("LoraRemoteKey").length() != AUTH_TOKEN_SIZE
      || CanCfg.getValue("LoraCarKey").length() != AUTH_TOKEN_SIZE)
    {
      s->println("Error: Please set 16 bytes Lora remote/car keys first");
      return ;
    }
  car.carKey(CanCfg.getValue("LoraCarKey").c_str());
  car.remoteKey(CanCfg.getValue("LoraRemoteKey").c_str());
  remote.carKey(CanCfg.getValue("LoraCarKey").c_str());
  remote.remoteKey(CanCfg.getValue("LoraRemoteKey").c_str());
  showbuff(s, "generating token from car: ", car.genToken());
  showbuff(s, "Double Check ", car.token());
  car.encryptWithCar();
  showbuff(s, "asked car to encrypt, got: ", car.challenge());
  showbuff(s, "Giving challenge to remote: ", remote.setChallenge(car.challenge()));
  remote.decryptWithCar();
  showbuff(s, "asked remote to decrypt, got: ", remote.token());
  remote.encryptWithRemote();
  showbuff(s, "asked remote to encrypt, got: ", remote.challenge());
  showbuff(s, "Giving challenge to car: ", car.setChallenge(remote.challenge()));
  car.decryptWithRemote();
  showbuff(s, "asked car to decrypt, got: ", car.token());
}

bool    lorasend(espShell *sh, Stream *s, char **args)
{
  unsigned long time;
  unsigned int  len;
  char          c;

  auth_testings(sh, s);	// testings
  return (true);	// testings
  if (args[1] == 0)
    {
      s->println("Usage: lorasend \"text to send\"");
      return (true);
    }
  len = strlen(args[1]);
  Serial1.write("\r\n", 2);     // if asleep ... sending fake order to wake up
  delay(100);                   // and wait a bit for it to get awake
  while (Serial1.available())   // flushing any remaining bytes
    c = Serial1.read();
  Serial1.write(args[1], len);
  Serial1.write("\r\n", 2);
  time = millis();
  while (Serial1.available() == 0)
    if ((millis() - time) > 1000)
      break ;
    else
      delay(10);
  c  = ((char)Serial1.read());
  if (c != 0x04)
  {
    s->print("Error: No Answer (0x");
    s->print(c, HEX);
    s->println(")");
    return (true);
  }
  time = millis();
  while (true)
    if (Serial1.available())
      {
        c = ((char)Serial1.read());
        if (c == 0x04)
          break ;
        s->print(c);
        time = millis();
      }
    else
      {
        if ((millis() - time) > 5000) // keep in mind that if command takes more than this value without writing data ... connection will be closed
          {
            s->println("Error: lost in transaction");
            break ;
          }
      }
  return (true);
}

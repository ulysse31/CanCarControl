#include "CanCarControl.h"

bool	enable_wifi(Stream *s)
{
  if (CanCarControl.loadWiFi(s) == false)
    {
      s->println("Could not turn on Wifi, is Wifi already on ?");
      return (false);
    }
  return (true);
}

bool	disable_wifi(Stream *s)
{
  if (CanCarControl.unloadWiFi(s) == false)
    {
      s->println("Could not turn off Wifi, is Wifi already off ?");
      return (false);
    }
  return (true);    
}

void	wifi_show(Stream *s)
{
  if (CanCarControl.isWiFiActive())
    {
      s->println("wifi:");
      s->println("\tUP");
      s->print("\tStatus:");
      s->print((WiFi.isConnected() ? "Connected" : "Disconnected"));
      s->print(" SSID: ");
      s->print(WiFi.SSID());
      s->print(" RSSI: ");
      s->println(WiFi.RSSI());
      s->print("\tether: ");
      s->println(String(WiFi.macAddress()));
      s->print("\tip: ");
      s->print(WiFi.localIP().toString());
      s->print(" netmask: ");
      s->println(WiFi.subnetMask().toString());
      s->print("\tgw: ");
      s->print(WiFi.gatewayIP().toString());
      s->print(" dns: ");
      WiFi.dnsIP().printTo(*s);
      s->print(", ");
      WiFi.dnsIP(1).printTo(*s);
      s->println();
      s->println();      
    }
  else
    {
      s->println("wifi:");
      s->println("\tDOWN");
      s->println();
      s->println();
    }
}


bool	ifconfig(espShell *sh, Stream *s, char **args)
{
  bool	show_wifi = false;

  if (args[1] == 0)
      show_wifi = true;
  if (args[1] && (strcmp(args[1], "wifi") != 0))
    {
      s->println("Usage: ifconfig [wifi up|down]");
      return (false);
    }
  if (args[1] && strcmp(args[1], "wifi") == 0 && args[2] && strcmp(args[2], "up") == 0)
    return (enable_wifi(s));
  if (args[1] && strcmp(args[1], "wifi") == 0 && args[2] && strcmp(args[2], "down") == 0)
    return (disable_wifi(s));
  if (args[1] && strcmp(args[1], "wifi") == 0 && args[2] == 0)
    show_wifi = true;
  if (show_wifi)
    wifi_show(s);
  return (true);
}


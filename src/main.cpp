#define _DECLARE_GLOBAL_CANCARCONTROL_
#include <CanCarControl.h>

void setup()
{
  LastActivity = millis();
  CanCarControl.init();
  CanCarControl.loadConfig();
  CanCarControl.loadServices();
}

void loop()
{
  CanCarControl.taskLoop();
  if (CanCarCfg.getValue("EnableSleep") == "true" && CanCarCfg.getValue("InactivityTimeout") != ""
      && (millis() - LastActivity) > (CanCarCfg.getValue("InactivityTimeout").toInt() * 1000))
    CanCarControl.goToSleep();
}

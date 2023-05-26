#define _DECLARE_GLOBAL_CANCARCONTROL_
#include <CanGlobal.h>

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
  if (CanCfg.getValue("EnableSleep") == "true" && CanCfg.getValue("InactivityTimeout") != ""
      && (millis() - LastActivity) > (CanCfg.getValue("InactivityTimeout").toInt() * 1000))
    CanCarControl.goToSleep();
}

#include "CanCarControl.h"

CanCarControl::CanCarControl()
{

}

CanCarControl::~CanCarControl()
{

}

void
CanCarControl::init()
{
  pinMode(E220_AUX, INPUT);
  _spiffsstatus = SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);
  pinMode(MCP_PWR, OUTPUT);
  pinMode(LUATOS_LED1, OUTPUT);
  pinMode(LUATOS_LED2, OUTPUT);
  digitalWrite(MCP_PWR, HIGH);
  digitalWrite(LUATOS_LED1, HIGH);
  Serial1.begin(LORA_SETUP_BAUD, SERIAL_8N1, LUATOS_RX1, LUATOS_TX1);
  Serial.begin(SERIAL_DEFAULT_SPEED);
  Serial.println("####################### CanCarControl INIT #######################");
  SPI.begin(LUATOS_SCK, LUATOS_MISO, LUATOS_MOSI, LUATOS_SS);
  if (mcp2515.begin(MCP_ANY, CAN_100KBPS, MCP_8MHZ) != CAN_OK)
    Serial.println("Error initializing MCP2515");
  mcp2515.setMode(MCP_NORMAL);
  shell = new espShell(&Serial);
  shellLoRa = new espShell(&Serial1, false, false, true);
  shellTelnet = 0;
  _wifiActive = false;
  _webActive = false;
  _telnetActive = false;
}

bool
CanCarControl::checkWiFiParams(Stream *callingStream)
{
  bool	ret = true;

  if (CanCarCfg.getValue("WiFiHost") == "")
    {
      callingStream->println("\t- WiFiHost not set, please set a Wireless Hostname");
      ret = false;
    }
  if (CanCarCfg.getValue("WiFiMode") == "")
    {
      callingStream->println("\t- WiFiMode not set, please set a Wireless Mode (either \"AP\" or \"STA\")");
      ret = false;
    }
  if (CanCarCfg.getValue("WiFiSSID") == "")
    {
      callingStream->println("\t- WiFiSSID not set, please set an SSID");
      ret = false;
    }
  if (CanCarCfg.getValue("WiFiPassphrase") == "")
    {
      callingStream->println("\t- WiFiPassphrase not set, please set a Wireless Passphase");
      ret = false;
    }
  return (ret);
}

bool
CanCarControl::loadWiFi(Stream *callingStream)
{
  unsigned long time;
  int		wfStatus;
  
  if (_wifiActive == true || checkWiFiParams(callingStream) == false)
    return (false);
  callingStream->println("");
  callingStream->print("Starting Wireless Network ");
  if (CanCarCfg.getValue("WiFiMode") == "STA")
    {
      WiFi.mode(WIFI_MODE_STA); // calls esp_wifi_set_mode(WIFI_MODE_STA); and esp_wifi_start();
      WiFi.enableSTA(true);
      int scanResult = WiFi.scanNetworks(); // doing scan
      if(scanResult > 0)
	WiFi.begin(CanCarCfg.getcValue("WiFiSSID"), CanCarCfg.getcValue("WiFiPassphrase"));
      time = millis();
      while ((wfStatus = WiFi.status()) != WL_CONNECTED && (millis() - time) < (1000*60*1)) // 1 min timeout
	{
	  delay(500);
	  callingStream->print(".");
	}
      if (wfStatus != WL_CONNECTED)
	{
	  callingStream->println("[Failed]");
	  callingStream->println("");
	  return (false);
	}
      callingStream->println("[OK]");
      callingStream->println("");
      if (CanCarCfg.getValue("EnableWebService") == "true")
	{
	  web.spiffsinit(_spiffsstatus);
	  callingStream->println("");
	  callingStream->print("Starting Web Service ...\t");
	  callingStream->println((web.spiffsinit() ? "[OK]" : "[Failed]"));
	  callingStream->println("");
	}
      callingStream->print("Starting MDNS Responder ...\t");
      if (!MDNS.begin(CanCarCfg.getcValue("WiFiHost")))
	{
	  callingStream->println("[Failed]");
	  //return (false);
	}
      callingStream->println("[OK]");
      callingStream->println("");
      if (CanCarCfg.getValue("EnableWebService") == "true")
	{
	  if (CanCarCfg.getValue("WWWuser") != "")
	    web.setuser(CanCarCfg.getcValue("WWWuser"));
	  if (CanCarCfg.getValue("WWWpass") != "")
	    web.setpass(CanCarCfg.getcValue("WWWpass"));
	  callingStream->print("Applying Web bindings ...\t");
	  web.bindings();
	  _webActive = true;
	  callingStream->println("[OK]");
	}
      if (CanCarCfg.getValue("EnableTelnetService") == "true" && _telnetActive == false)
	{
	  callingStream->print("Starting Telnet Service... ");
	  TelnetStream.begin();
	  callingStream->println("[OK]");
	  /*
	  if (TelnetStream.begin())
	    callingStream->println("[OK]");
	  else
	    callingStream->println("[Failed]");
	  */
	  shellTelnet = new espShell(&TelnetStream, false);
	  _telnetActive = true;
	}
      this->_wifiActive = true;
      return (true);
    }
  else
    callingStream->println("Only STA mode implemented for now ... (duhhh....)");
  return (false);
}

bool
CanCarControl::unloadWiFi(Stream *callingStream)
{
  if (_wifiActive == false)
    return (false);
  //
  if (_webActive)
    {
      callingStream->print("Stopping Web Service ...\t");
      web.stop();
      callingStream->println("[OK]");
    }
  if (_telnetActive)
    {
      TelnetStream.stop();
      delete shellTelnet;
      shellTelnet = 0;
      _telnetActive = false;
    }
  callingStream->print("Disconnecting Wireless ...\t");
  WiFi.disconnect(true);
  callingStream->println("[OK]");
  callingStream->print("Powering Off WiFi ...\t");
  WiFi.mode(WIFI_OFF);
  callingStream->println("[OK]");
  this->_wifiActive = false;
  return (true);
}

bool
CanCarControl::getYesNo(const char *Message)
{
  char c;

  Serial.print(Message);
  Serial.print(" (Y/N): ");
  while (true)
    {
      if (Serial.available() > 0)
      {
        c = Serial.read();
        if (c == 'y' || c == 'Y')
          return (true);
        if (c == 'n' || c == 'N')
          return (false);
      }
      else
        delay(200);
    }
  return (false);
}

String
CanCarControl::getParam(const char *param_name, bool confirm)
{
  char    c = 0;
  String  result;
  String  strconf;
  bool    inLoop = true;

  Serial.print(param_name);
  while (inLoop)
  {
    result = "";
    while (c != '\r' && c != '\n')
    {
      if (Serial.available() > 0)
      {
        c = Serial.read();
        Serial.print(c);
        result += c;
      }
      else
        delay (100);
    }
    result[result.length() - 1] = '\0';
    Serial.print("\r\n");
    strconf = result;
    strconf = strconf + ", confirm ?";
    (getYesNo(strconf.c_str()) ? inLoop = false : inLoop = true);
  }
  result[result.length() - 1] = '\0';
  return (result);
}

void
CanCarControl::loadCfgSerial()
{
  Serial.println("Interactive Config menu setting launched:");
  Serial.print("\r\n");

  Serial.setTimeout(400000000);
  String host = getParam("\r\nEnter host name: ", true);
  String ssid = getParam("\r\nEnter SSID: ", true);
  String passphrase = getParam("\r\nEnter Passphrase: ", true);

  CanCarCfg.setValue("host", host.c_str());
  CanCarCfg.setValue("ssid", ssid.c_str());
  CanCarCfg.setValue("passphrase", passphrase.c_str());
  Serial.setTimeout(1000);
  Serial.println("All done! Saving ...");
  if(CanCarCfg.saveCfg() == true)
    Serial.println("Configuration Successfuly Saved!\n");
  else
    Serial.println("Failed to Save Configuration!\n");
}



void
CanCarControl::loadConfig()
{
  CanCarCfg.loadCfg();
  Aliases.loadCfg();
  //  if (!cfgstatus || Cfg.getValue("ssid").length() == 0)
  //loadCfgSerial();
  //Serial.print("CfgLoad:ssid:");
  //Serial.println(CanCarCfg.getValue("ssid"));
  // Connect to WiFi network
}

void
CanCarControl::loadServices()
{
  Serial.println("Now dropping shells...");
  shell->init();
  shellLoRa->init();
}

void
CanCarControl::taskLoop()
{
  shell->checkCmdLine();
  shellLoRa->checkCmdLine();
  if (this->isWiFiActive())
    {
      if (this->isWebActive())
	web.handleClient();
      if (this->isTelnetActive())
	{
	  shellTelnet->checkCmdLine();
	  TelnetStream.flush();
	}
    }
  delay(10);
}


void
CanCarControl::goToSleep()
{

   if (esp_deep_sleep_enable_gpio_wakeup( 1 << E220_AUX, ESP_GPIO_WAKEUP_GPIO_LOW) != ESP_OK)
     {
       Serial.println("\nError: Could not enable AUX wakeup pin");
       LastActivity = millis();
       return ;
     }
   Serial.println("\n****************\n* Inactivity Timeout\n*  Going to Sleep\n****************\n");
   esp_deep_sleep_start();
}

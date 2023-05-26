#ifndef __CANCARCONTROL_H__
# define __CANCARCONTROL_H__

# ifndef __ESPMotics_WEBSERVER_H__
#  include "ESPMoticsWebServer.h"
#endif

# include <stdint.h>
//# include <WProgram.h>
# include <Arduino.h>
# include <SPI.h>
# include <FS.h>
# include <SPIFFS.h>
# include <mcp_can.h>
# include <WiFi.h>
# include <Wire.h>
# include <WiFiClient.h>
# include <WebServer.h>
# include <TelnetStream.h>
# include <ESPmDNS.h>
# include <Update.h>
# include "mbedtls/aes.h"
# include "soc/soc.h" //disable brownour problems
# include "soc/rtc_cntl_reg.h" //disable brownour problems
# include "ESPMoticsCfg.h"
# include "authToken.h"
# include "espShell.h"
# include "result.h"
# include "xmodem_receiver.h"
# include "cppfix.h"

# define FORMAT_SPIFFS_IF_FAILED true

# define CFGFILE "Can.cfg"
# define ALIASFILE "Aliases.cfg"


# define DEBUGMODE
//# define DEBUG_MODE	1

//# define SERIAL_DEFAULT_SPEED	57600
//# define SERIAL_DEFAULT_SPEED	38400
# define SERIAL_DEFAULT_SPEED	115200
//# define SERIAL_DEFAULT_SPEED	230400
//# define SERIAL_DEFAULT_SPEED	460800
# define LORA_SERIAL		Serial1
# define LORA_SETUP_BAUD	9600
/*
# define LUATOS_SCK		2
# define LUATOS_MOSI		3
# define LUATOS_MISO		10
# define LUATOS_SS		7
# define LUATOS_RX1		1
# define LUATOS_TX1		0
# define LUATOS_RX0		20
# define LUATOS_TX0		21
# define LUATOS_LED1		12
# define LUATOS_LED2		13

# define MCP_PWR		8
# define MCP_INT		12
# define MCP_CS			LUATOS_SS
# define E220_AUX		5
# define E220_M0		6
# define E220_M1		4
*/
# define LUATOS_SCK		GPIO_NUM_2
# define LUATOS_MOSI		GPIO_NUM_3
# define LUATOS_MISO		GPIO_NUM_10
# define LUATOS_SS		GPIO_NUM_7
# define LUATOS_RX1		GPIO_NUM_1
# define LUATOS_TX1		GPIO_NUM_0
# define LUATOS_RX0		GPIO_NUM_20
# define LUATOS_TX0		GPIO_NUM_21
# define LUATOS_LED1		GPIO_NUM_12
# define LUATOS_LED2		GPIO_NUM_13

# define MCP_PWR		GPIO_NUM_12
//# define MCP_INT		GPIO_NUM_12
# define MCP_CS			LUATOS_SS
# define E220_AUX		GPIO_NUM_5
# define E220_M0		GPIO_NUM_6
# define E220_M1		GPIO_NUM_4

// 3 minutes for now
//# define ACTIVITY_TIMEOUT	20000
// for debug
# define ACTIVITY_TIMEOUT	9990000

# ifndef DEBUGMODE
#  define CMDSERIAL	Serial
# endif

#ifdef DEBUGMODE
# define DBGSERIAL	Serial
# define CMDSERIAL	DBGSERIAL
# define DEBUGINIT	DBGSERIAL.begin(9600)
# define PRINTDBG(x) do { DBGSERIAL.print("["); DBGSERIAL.print(__FILE__); DBGSERIAL.print(":"); DBGSERIAL.print(__LINE__); DBGSERIAL.print("] "); DBGSERIAL.println(x); } while (false)
#else
# define PRINTDBG(x)
#endif

# define CMDINIT	CMDSERIAL.begin(SERIAL_DEFAULT_SPEED)
# define PRINTCMD(x) do { CMDSERIAL.print("["); CMDSERIAL.print(__FILE__); CMDSERIAL.print(":"); CMDSERIAL.print(__LINE__); CMDSERIAL.print("] "); CMDSERIAL.println(x); } while (false)

class CanCarControl;



# ifdef _DECLARE_GLOBAL_CANCARCONTROL_
ESPMoticsCfg            CanCfg(CFGFILE, '=');
ESPMoticsCfg            Aliases(ALIASFILE, '=');
ESPMoticsWebServer      web(80);
espShell *		shell;
espShell *		shellLoRa;
espShell *		shellTelnet;
//ESPTelnetStream		TelnetStream;
MCP_CAN			mcp2515(MCP_CS);
unsigned long		LastActivity;
#  else
extern ESPMoticsCfg		CanCfg;
extern ESPMoticsCfg		Aliases;
extern espShell *		shell;
extern espShell *		shellLoRa;
extern espShell *		shellTelnet;
//extern ESPTelnetStream		TelnetStream;
extern MCP_CAN			mcp2515;
extern unsigned long		LastActivity;
#  ifndef _DECLARE_GLOBAL_WEBSRV_
extern ESPMoticsWebServer  web;
#  endif
# endif

class   CanCarControl
{
public:
  CanCarControl();
  ~CanCarControl();
  String		getParam(const char *param_name, bool confirm = true);
  void			loadCfgSerial();
  bool			getYesNo(const char *Message);
  void			init();
  bool                  loadWiFi(Stream *callingStream);
  bool                  checkWiFiParams(Stream *callingStream);
  bool                  unloadWiFi(Stream *callingStream);
  bool                  isWiFiActive() { return (_wifiActive); }
  bool			isWebActive() { return (_webActive); }
  bool			isTelnetActive() { return (_telnetActive); }
  void			loadConfig();
  void			loadServices();
  void			taskLoop();
  void			goToSleep();

private:
  bool			_spiffsstatus;
  bool                  _wifiActive;
  bool			_webActive;
  bool			_telnetActive;
};

# ifdef _DECLARE_GLOBAL_CANCARCONTROL_
CanCarControl		CanCarControl;
# else
extern CanCarControl	CanCarControl;
# endif

#endif // __CANCARCONTROL__ //

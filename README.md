# CanCarControl
CanBus car control device: read, interpret &amp; inject CAN Bus messages on your car.  
This project uses :
* A LuatOS ESP32-C3 board
* A MCP2515 module board
* A E220-400T22D board (or, any other 7 pin LoRa EByte module)
* A LM2596S module board as power supply (regulated to 5V) 

The 6 pin connector feats the BMW e87 CID display connector.  

for the schematics of CanCarControl, please visit:  

https://oshwlab.com/ulysse31/espcan_car

You can find the remote control project here:  

https://github.com/ulysse31/CanRemote

## How it works
While creating this projet, I started thinking on a really basic and light shell that would ease interaction with the device and the car CAN-bus...  
The goal at first was to be able to execute different actions on the CAN-bus (like dumping, injecting ...) ... I quickly found myself limited needing to store settings ... so then I created the ESPMoticsCfg class ... then ... to manage the files in the SPIFFS ... I added some commands to manage the filesystem via the shell ... them other commands for Lora ...  
NB: THE SHELL DO NOT HAVE TERMCAPS SUPPORT => you type your command, if you need to correct, backspace is (the only) supported feature ...  
In the end, the shell on this device (CanCarControl) has (for now) the following commands:

  | Command | Category | Description |
  | ------- | ------ | ----------------- |
  | cat | fs | show file content |
  | ls | fs | list files |
  | rm | fs | delete a file |
  | cp | fs | copy a file |
  | mv | fs | rename a file |
  | ed | fs | basic "per line file editor" (NO TERMCAPS/CURSOR ACTION, ONLY typing and BACKSPACE if needed) |
  | md5sum | sys | calculates the md5 of a file (useful for OTA updates) |
  | xmreceive | sys | a command to receive a file via xmodem protocol over the serial |
  | fwupdate | sys | proceed to OTA with the given file |
  | ifconfig | sys | allows to bring wifi up/down (required parameters needs to be set in the configuration settings) |
  | cfg | sys | command to manage main configuration parameters |
  | alias | sys | alias command / shortcuts management tool |
  | pin | sys | gpio pin management => allows to set a pin mode, read and/or write HIGH / LOW |
  | sleep | sys | command shortcut to manage sleep configuration parameters => after setting them you still must do a "cfg save" to keep settings at reboot |
  | free | sys | shows free memory (useful for hunting memory leaks) |
  | serial | sys | serial management tool => allow to connect from a serial to another serial, at a given speed ... |
  | exec | sys | execute commands from a given file |
  | delay | sys | waits x ms then ends ... |
  | restart | sys | restart the device |
  | candump | CAN | command interface for dumping CAN-Bus |
  | canwait | CAN | waits during a given time for a list of CAN-IDS, useful when raw flow is huge (often the case) |
  | canwrite | CAN | injects CAN-bus messages into CAN-bus Network, from arguments or from a given file |
  | interactive | LoRa | enable/disable shell "interactiveness" (echo, verbose) => the lora serial uses a non-interactive shell ... |
  | lorasend | LoRa | sends a command to distant lora shell |
  | lorasecure | LoRa | enable / disables authentication system on local listening lora shell |

## System config
A basic configuration file named /etc/Can.cfg on the SPIFFS contains all the common variable settings.  
PLEASE REMEMBER THAT SPIFFS DO NOT SUPPORT DIRECTORIES...  so it is one file named /etc/Can.cfg yes ...  
Here is a list of parameters used actually:
  |    Variable Name    |   Value Type   |                        Description                        |
  | ------------------- | -------------- | --------------------------------------------------------- |
  | LoraSecure          |  true / false  | Boolean string to disable/enable auth in local lora shell |
  | LoraRemoteKey       | 16 char string | AES-128 bit string used as key by the remote device       |
  | LoraCarKey          | 16 char string | AES-128 bit string used as key by CanCarControl device    |
  | WiFiMode            |    string      | wifi mode to use, only "STA" is supported for now         |
  | WiFiSSID            |    string      | Wireless SSID to join when wifi is enabled                |
  | WiFiPassphrase      |    string      | Wireless PassPhrase to use                                |
  | WiFiHost            |    string      | Hostname to use while joining wireless                    |
  | EnableSleep         |  true / false  | Boolean to enable / disable sleep feature                 |
  | InactivityTimeout   |  num. string   | number of seconds of inactivity before going to sleep     |
  | WWWuser             |    string      | login to use in web service authentication                |
  | WWWpass             |    string      | password to use in web service authentication             |
  

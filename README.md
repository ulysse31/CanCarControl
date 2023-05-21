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
In the end, the shell on this device (CanCarControl) has (for now) the following commands:
  * cat: show file content
  * ls: list files
  * rm: delete a file
  * cp: copy a file
  * mv: rename a file
  * ed: basic "per line file editor" (NO TERMCAPS/CURSOR ACTION, ONLY typing and BACKSPACE if needed)
  * md5sum: calculates the md5 of a file (useful for OTA updates)
  * xmreceive: a command to receive a file via xmodem protocol over the serial
  * candump: command interface for dumping CAN-Bus
  * canwait: waits during a given time for a list of CAN-IDS, useful when raw flow is huge (often the case)
  * fwupdate: proceed to OTA with the given file
  * ifconfig: allows to bring wifi up/down (required parameters needs to be set in the configuration settings)
  * interactive: enable/disable shell "interactiveness" (echo, verbose) => the lora serial uses a non-interactive shell ...
  * cfg: command to manage main configuration parameters
  * alias: alias command / shortcuts management tool
  * pin: gpio pin management => allows to set a pin mode, read and/or write HIGH / LOW
  * sleep: command shortcut to manage sleep configuration parameters => after setting them you still must do a "cfg save" to keep settings at reboot
  * free: shows free memory (useful for hunting memory leaks)
  * serial: serial management tool => allow to connect from a serial to another serial, at a given speed ...
  * exec: execute commands from a given file
  * delay: waits x ms then ends ...
  * lorasend: sends a command to distant lora shell
  * lorasecure: enable / disables authentication system on local listening lora shell
  * restart: restart the device

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
  

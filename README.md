# CanCarControl
CanBus car control device: read, interpret &amp; inject CAN Bus messages on your car.  
The goal of this project is to play with the car, with a "DIY"/"Homemade" remote (other repo) in order to do stuff such as:  
* Locking/unlocking the car
* Get the GPS infos
* Enable/disable the warnings
* Opening closing the windows
* Get the windows and door status
All this remotely (of course) within a range of 2/3 kilometers ^^" (thanks LoRa)  
  
  
This project uses the following hardware:
* A LuatOS ESP32-C3 board
* A MCP2515 module board
* A E220-400T22D board (or, any other 7 pin LoRa EByte module)
* A LM2596S module board as power supply (regulated to 5V) 

The 6 pin connector feats the BMW e87 CID display connector (this is for ease in my car, but fill free to take my schematics and make your own).  
  
For the schematics of CanCarControl, please visit:  

https://oshwlab.com/ulysse31/espcan_car

You can find the remote control project here:  

https://github.com/ulysse31/CanRemote

## How it works
While creating this projet, I started thinking on a really basic and light shell that would ease interaction with the device and the car CAN-bus... it was more like a dev-tool at first ... that I finally decided to keep on the project on it own ^^' ...  
The goal at first was to be able to execute different actions on the CAN-bus (like dumping, injecting ...) ... I quickly found myself limited needing to store settings ... so then I created the ESPMoticsCfg class ... then ... to manage the files in the SPIFFS ... I added some commands to manage the filesystem via the shell ... them other commands for Lora ... in the end its quite consequent now ^^' ... oh well ... at least I can re-use that on other projects ^^' ...  
  
NB: THE SHELL DO NOT HAVE TERMCAPS SUPPORT => you type your command, if you need to correct, backspace is (the only) supported feature ...  
  
NB2: please keep in mind that the goal of this project was NOT the shell nor the config class ... so the code canbe considered clearly wrote "quick & dirty" / "but it works" ... ^^" ...  
  
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
  
## How it (concretely) works
this "mini-system" uses variables and shell commands to do the needed actions, to get things working, you first need to identify the CAN-Bus messages that you want to either analyze or inject.   
#### On this section we'll take a concret example : trying to lock the car on our own.  
we first need to identify the locking sequence, on my side, I did not had any documentation from the car manufacturer (BMW) to know exactly what what the usable message, so had to do some internet digging, and also used the candump command, which after dumping the CAN-Bus while locking/unlocking multiple times the car (and counting how much locks/unlocks were made).  
candump will dump all content passing on the CAN-Bus network, the output content is like :  
>[...]  
>7919,0x1A0,0x8,0x0,0x80,0x0,0x0,0x80,0x0,0x18,0xBA  
>7943,0x1D0,0x8,0x83,0xFF,0x49,0xCE,0x0,0x0,0xFD,0xA6  
>7953,0xA8,0x8,0xE5,0x2C,0xFD,0x20,0xFD,0xF1,0x3,0x0  
>7954,0xAA,0x8,0x58,0x2B,0xFD,0x0,0x0,0x0,0x84,0x0  
>7967,0x4F8,0x8,0x0,0x52,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  
>7970,0x130,0x5,0x0,0x1,0x14,0xF,0x4A  
>[...]  
  
The output is csv compliant, and is composed of the following columns : timestamp (in ms), CAN-ID, data-length,byte1,byte2,byte3...  
| timestamp (in ms) | CAN-MsgID | data length | data bytes |
| ----------------- | --------- | ----------- | ------ |
  
After some testing, and for my car, I discovered that there was multiple types of locks (central locking vs remote/external/real locking ...), and had to differentiate ... anyways ... the sequence that I wanted something line was :
  
>[...]  
>2009,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>2114,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>2215,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>2445,0x2A0,0x8,0x88,0x88,0x88,0x1,0xE,0x43,0x26,0x3  
>[...]  
  
So I created a file named "lock" on the SPIFFS (using ed command), containing the following :
  
>9,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>114,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>215,0x2A0,0x8,0x22,0x22,0x18,0x1,0xE,0x43,0x26,0x3  
>445,0x2A0,0x8,0x88,0x88,0x88,0x1,0xE,0x43,0x26,0x3  
  
Tested it typing command "canwrite -f lock" ... and the car locked itself ...  
Now, I create an alias named "lock" with command "canwrite -f lock":  
>alias lock "canwrite -f lock"
Test my alias typing "lock" as command ... it worked ! ^^'  
finally save my alias:  

>alias save

This allows the alias to be loaded at boot, and we are done ... we just need then to tell the remote to send to the lora shell the "lock" command via the command:  
  
> lorasend lock
  
On the remote shell (or via its screen GUI, which launches the shell command ...) and the remote will lock the car ^^'  

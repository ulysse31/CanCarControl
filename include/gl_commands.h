#ifndef __GL_COMMANDS_H__
# define __GL_COMMANDS_H__

t_cmdfunc       gl_commands[] = {
  { "cat", "show file content", cat },
  { "ls", "list files", ls },
  { "rm", "delete file", rm },
  { "cp", "copy file", cp },
  { "mv", "rename file", cp },
  { "ed", "a line-per-line basic text editor", ed },
  { "md5sum", "calculates md5 of a file", md5sum },
  { "xmreceive", "receive file from tty using XMODEM protocol", xmreceive },
  { "candump", "CAN Dumper", candump },
  { "canwait", "CAN packet waiting for specific identifiers", canwait },
  { "canwrite", "CAN packet writing: from arguments", canwrite },
  { "fwupdate", "firmware update command", fwupdate },
  { "ifconfig", "show network connection states", ifconfig },
  { "interactive", "enable/disable interactive shell parameters (prompt, verbosity...)", interactive },
  { "setecho", "enable/disable character echoing", setecho },
  { "cfg", "main config parameters editor", cfg },
  { "alias", "alias command / shortcuts", alias },
  { "pin", "pin input/output control", pin },
  { "sleep", "sleep status/management command", cmd_sleep },
  { "free", "show free memory", cmd_free },
  { "echo", "echo text", echo },
  { "serial", "serial redirection tool", serial },
  { "exec", "execute commands from file", exec },
  { "doorstatus", "waits and extract door status from CAN-Bus GPS data", doorstatus },
  { "gps", "waits and extract GPS positioning from CAN-Bus GPS data", gps },
  { "delay", "sleeps for a specified number of milliseconds", cmd_delay },
  { "lorasend", "send command via lora", lorasend },
  { "lorasecure", "enable/disable auth in lorashell", lorasecure },
  { "restart", "restart the device", restart },
  {0, 0, 0}
};

#endif // !__GL_COMMANDS_H__ //

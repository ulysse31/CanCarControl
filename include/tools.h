#ifndef __TOOLS_H__
# define __TOOLS_H__

const char * getfullpath(const char *ptr);

bool    cat(espShell *sh, Stream *s, char **args);
bool    ls(espShell *sh, Stream *s, char **args);
bool    rm(espShell *sh, Stream *s, char **args);
bool    cp(espShell *sh, Stream *s, char **args);
bool    mv(espShell *sh, Stream *s, char **args);
bool    ed(espShell *sh, Stream *s, char **args);
bool    md5sum(espShell *sh, Stream *s, char **args);
bool    xmreceive(espShell *sh, Stream *s, char **args);
bool    candump(espShell *sh, Stream *s, char **args);
bool    canwait(espShell *sh, Stream *s, char **args);
bool    canwrite(espShell *sh, Stream *s, char **args);
bool    canwritefile(espShell *sh, Stream *s, char **args);
bool    fwupdate(espShell *sh, Stream *s, char **args);
bool    interactive(espShell *sh, Stream *s, char **args);
bool    setecho(espShell *sh, Stream *s, char **args);
bool    ifconfig(espShell *sh, Stream *s, char **args);
bool    lorasend(espShell *sh, Stream *s, char **args);
bool    lorasecure(espShell *sh, Stream *s, char **args);
bool    cfg(espShell *sh, Stream *s, char **args);
bool    alias(espShell *sh, Stream *s, char **args);
bool    pin(espShell *sh, Stream *s, char **args);
bool    serial(espShell *sh, Stream *s, char **args);
bool    exec(espShell *sh, Stream *s, char **args);
bool    doorstatus(espShell *sh, Stream *s, char **args);
bool    gps(espShell *sh, Stream *s, char **args);
bool    cmd_delay(espShell *sh, Stream *s, char **args);
bool    cmd_sleep(espShell *sh, Stream *s, char **args);
bool    cmd_free(espShell *sh, Stream *s, char **args);
bool    echo(espShell *sh, Stream *s, char **args);
bool    restart(espShell *sh, Stream *s, char **args);
void    showbuff(Stream *s, const char *title, const char *buff);
void    showbuffhex(Stream *s, const char *title, const char *buff);

#endif // !__TOOLS_H__ //

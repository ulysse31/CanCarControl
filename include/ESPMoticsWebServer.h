#ifndef __ESPMotics_WEBSERVER_H__
# define __ESPMotics_WEBSERVER_H__
# ifndef WEBSERVER_H
#  include <WebServer.h>
# endif
# ifndef ESP8266UPDATER_H
#  include <Update.h>
# endif
# define SPIFFS_CACHE 0
# define FORMAT_SPIFFS_IF_FAILED true
# include "SPIFFS.h"

#include "CanCarControl.h"
# define BUFFLEN    256 //SPIFFS’ 256 byte minimum file allocation unit

class ESPMoticsWebServer;
//
// Global Variable declarations
//
#define WEBUSER  "espcan"
#define WEBPASS  "espcan"

# ifdef _DECLARE_GLOBAL_WEBSRV_
ESPMoticsWebServer         *espweb;

const char* headerIndex =
    "<!DOCTYPE html>\n\
    <html lang=\"en-US\">\n\
    <head>\n\
    <style>\n\
body {\n\
    -ms-text-size-adjust:100%;\n\
    -webkit-text-size-adjust:100%;\n\
    line-height:1.5;\n\
    color:#24292e;\n\
    font-family:-apple-system,BlinkMacSystemFont,\"Segoe UI\",Helvetica,Arial,sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\";\n\
    font-size:16px;\n\
    line-height:1.5;\n\
    word-wrap:break-word\n\
}\n\
a{\n\
    background-color:transparent\n\
}\n\
a:active,a:hover{\n\
    outline-width:0\n\
}\n\
strong{\n\
    font-weight:bolder\n\
}\n\
h1{\n\
    font-size:2em;\n\
    margin:0.67em 0\n\
}\n\
img{\n\
    border-style:none\n\
}\n\
pre{\n\
    font-family:monospace,monospace;\n\
    font-size:1em\n\
}\n\
hr{\n\
    box-sizing:content-box;\n\
    height:0;\n\
    overflow:visible\n\
}\n\
input{\n\
    font:inherit;\n\
    margin:0\n\
}\n\
input{\n\
    overflow:visible\n\
}\n\
body[type=\"checkbox\"]{\n\
    box-sizing:border-box;\n\
    padding:0\n\
}\n\
*{\n\
    box-sizing:border-box\n\
}\n\
input{\n\
    font-family:inherit;\n\
    font-size:inherit;\n\
    line-height:inherit\n\
}\n\
a{\n\
    color:#0366d6;\n\
    text-decoration:none\n\
}\n\
a:hover{\n\
    text-decoration:underline\n\
}\n\
strong{\n\
    font-weight:600\n\
}\n\
hr{\n\
    height:0;\n\
    margin:15px 0;\n\
    overflow:hidden;\n\
    background:transparent;\n\
    border:0;\n\
    border-bottom:1px solid #dfe2e5\n\
}\n\
hr::before{\n\
    display:table;\n\
    content:\"\"\n\
}\n\
hr::after{\n\
    display:table;\n\
    clear:both;\n\
    content:\"\"\n\
}\n\
table{\n\
    border-spacing:0;\n\
    border-collapse:collapse\n\
}\n\
td,th{\n\
    padding:0\n\
}\n\
h1,h2,h3,h4,h5,h6{\n\
    margin-top:0;\n\
    margin-bottom:0\n\
}\n\
h1{\n\
    font-size:32px;\n\
    font-weight:600\n\
}\n\
h2{\n\
    font-size:24px;\n\
    font-weight:600\n\
}\n\
h3{\n\
    font-size:20px;\n\
    font-weight:600\n\
}\n\
h4{\n\
    font-size:16px;\n\
    font-weight:600\n\
}\n\
h5{\n\
    font-size:14px;\n\
    font-weight:600\n\
}\n\
h6{\n\
    font-size:12px;\n\
    font-weight:600\n\
}\n\
p{\n\
    margin-top:0;\n\
    margin-bottom:10px\n\
}\n\
blockquote{\n\
    margin:0\n\
}\n\
ul,ol{\n\
    padding-left:0;\n\
    margin-top:0;\n\
    margin-bottom:0\n\
}\n\
ol ol,ul ol{\n\
    list-style-type:lower-roman\n\
}\n\
ul ul ol,ul ol ol,ol ul ol,ol ol ol{\n\
    list-style-type:lower-alpha\n\
}\n\
dd{\n\
    margin-left:0\n\
}\n\
pre{\n\
    margin-top:0;\n\
    margin-bottom:0;\n\
    font-family:\"SFMono-Regular\",Consolas,\"Liberation Mono\",Menlo,Courier,monospace;\n\
    font-size:12px\n\
}\n\
body::before{\n\
    display:table;\n\
    content:\"\"\n\
}\n\
body::after{\n\
    display:table;\n\
    clear:both;\n\
    content:\"\"\n\
}\n\
body>*:first-child{\n\
    margin-top:0!important\n\
}\n\
body>*:last-child{\n\
    margin-bottom:0!important\n\
}\n\
a:not([href]){\n\
    color:inherit;\n\
    text-decoration:none\n\
}\n\
p,blockquote,ul,ol,dl,table,pre{\n\
    margin-top:0;\n\
    margin-bottom:16px\n\
}\n\
hr{\n\
    height:0.25em;\n\
    padding:0;\n\
    margin:24px 0;\n\
    background-color:#e1e4e8;\n\
    border:0\n\
}\n\
blockquote{\n\
    padding:0 1em;\n\
    color:#6a737d;\n\
    border-left:0.25em solid #dfe2e5\n\
}\n\
blockquote>:first-child{\n\
    margin-top:0\n\
}\n\
blockquote>:last-child{\n\
    margin-bottom:0\n\
}\n\
h1,h2,h3,h4,h5,h6{\n\
    margin-top:24px;\n\
    margin-bottom:16px;\n\
    font-weight:600;\n\
    line-height:1.25\n\
}\n\
h1{\n\
    padding-bottom:0.3em;\n\
    font-size:2em;\n\
    border-bottom:1px solid #eaecef\n\
}\n\
h2{\n\
    padding-bottom:0.3em;\n\
    font-size:1.5em;\n\
    border-bottom:1px solid #eaecef\n\
}\n\
h3{\n\
    font-size:1.25em\n\
}\n\
h4{\n\
    font-size:1em\n\
}\n\
h5{\n\
    font-size:0.875em\n\
}\n\
h6{\n\
    font-size:0.85em;\n\
    color:#6a737d\n\
}\n\
ul,ol{\n\
    padding-left:2em\n\
}\n\
ul ul,ul ol,ol ol,ol ul{\n\
    margin-top:0;\n\
    margin-bottom:0\n\
}\n\
li{\n\
    word-wrap:break-word\n\
}\n\
li>p{\n\
    margin-top:16px\n\
}\n\
li+li{\n\
    margin-top:0.25em\n\
}\n\
dl{\n\
    padding:0\n\
}\n\
dl dt{\n\
    padding:0;\n\
    margin-top:16px;\n\
    font-size:1em;\n\
    font-style:italic;\n\
    font-weight:600\n\
}\n\
dl dd{\n\
    padding:0 16px;\n\
    margin-bottom:16px\n\
}\n\
table{\n\
    display:block;\n\
    width:100%;\n\
    overflow:auto\n\
}\n\
table th{\n\
    font-weight:600\n\
}\n\
table th,table td{\n\
    padding:6px 13px;\n\
    border:1px solid #dfe2e5\n\
}\n\
table tr{\n\
    background-color:#fff;\n\
    border-top:1px solid #c6cbd1\n\
}\n\
table tr:nth-child(2n){\n\
    background-color:#f6f8fa\n\
}\n\
img{\n\
    max-width:100%;\n\
    box-sizing:content-box;\n\
    background-color:#fff\n\
}\n\
img[align=right]{\n\
    padding-left:20px\n\
}\n\
img[align=left]{\n\
    padding-right:20px\n\
}\n\
pre{\n\
    padding:16px;\n\
    overflow:auto;\n\
    font-size:85%;\n\
    line-height:1.45;\n\
    background-color:#f6f8fa;\n\
    border-radius:3px;\n\
    word-wrap:normal\n\
}\n\
body:checked+.radio-label{\n\
    position:relative;\n\
    z-index:1;\n\
    border-color:#0366d6\n\
}\n\
hr{\n\
    border-bottom-color:#eee\n\
}\n\
#log .v{\n\
    color:#888\n\
}\n\
#log .d{\n\
    color:#0DD\n\
}\n\
#log .c{\n\
    color:magenta\n\
}\n\
#log .i{\n\
    color:limegreen\n\
}\n\
#log .w{\n\
    color:yellow\n\
}\n\
#log .e{\n\
    color:red;\n\
    font-weight:bold\n\
}\n\
#log{\n\
    background-color:#1c1c1c\n\
}\n\
    </style>\n\
	<title>ESPDetector Web Interface</title>\n\
    </head>\n\
    <body>\n";

const char* footerIndex =
    "</body>\n\
</html>\n";

const char* menuIndex = 
"<div>\n"
    "<table width='100%'>\n"
        "<tr>\n"
            "<td colspan=2>\n"
                "<a href='/files'>Flash Storage</a>\n"
            "</td>\n"
            "<td colspan=2>\n"
                "<a href='/firmware'>Update Firmware</a>\n"
            "</td>\n"
        "</tr>\n"
    "</table>\n"
"</div>\n"
;

#  else
extern ESPMoticsWebServer         *espweb;
# endif

# define MAX_USERPASS_BUFF		32

//!@Brief: ESPMotics web server class (Webserver Wrapper) 
class ESPMoticsWebServer : public WebServer
{
public:
  ESPMoticsWebServer(int port);
  ~ESPMoticsWebServer();
  void              bindings();
  bool              spiffsinit() { return (_spiffsinit); }
  bool              spiffsinit(bool state) { _spiffsinit = state; return (_spiffsinit); }
  const char *	    getuser() { return (_webuser); }
  const char *	    getpass() { return (_webpass); }
  //void		    setuser(const char *user) { memset(_webuser, 0, sizeof(*_webuser) * (MAX_USERPASS_BUFF+1)); for (int i = 0; user[i] && i < MAX_USERPASS_BUFF; i++) _webuser[i] = user[i]; }
  //void		    setpass(const char *pass) { memset(_webpass, 0, sizeof(*_webpass) * (MAX_USERPASS_BUFF+1)); for (int i = 0; pass[i] && i < MAX_USERPASS_BUFF; i++) _webpass[i] = pass[i]; }
  void		    setuser(const char *user) { _webuser = user; }
  void		    setpass(const char *pass) { _webpass = pass; }
  void              on_webroot();
  void              do_upload();
  void              on_delete();
  void              on_format();
  void              on_rename();
  void              on_download();
  void              on_uploadurl();
  void              on_updatemenu();
  void              on_updateurl();
  void              on_fupdate();
  void		    on_filelist();
  void              do_update();

private:
  //  char		      _webuser[MAX_USERPASS_BUFF + 1];
  //  char		      _webpass[MAX_USERPASS_BUFF + 1];
  const char *	      _webuser;
  const char *	      _webpass;
  
  const char *        _menuIndex;
  const char *        _updateIndex;
  const char *        _headerIndex;
  const char *        _footerIndex;
  bool		      _spiffsinit;
  File                _fw;
};


// 
// End Global Variable declarations 
//


#endif // !__ESPMotics_WEBSERVER_H__ //

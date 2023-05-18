# define _DECLARE_GLOBAL_WEBSRV_

#include "WiFiClient.h"
#include "ESPMoticsWebServer.h"

ESPMoticsWebServer::ESPMoticsWebServer(int port) : WebServer(port)
{
    espweb = this;
    this->setuser(WEBUSER);
    this->setpass(WEBPASS);
    _menuIndex = menuIndex;
    _headerIndex = headerIndex;
    _footerIndex = footerIndex;
}

ESPMoticsWebServer::~ESPMoticsWebServer()
{

}

void
ESPMoticsWebServer::on_webroot()
{
  String page;

  sendHeader("Connection", "close");
  page += _headerIndex;
  page += _menuIndex;
  page += _footerIndex;
  send(200, "text/html", page);
}

void
ESPMoticsWebServer::on_updatemenu()
{
  String page;

  sendHeader("Connection", "close");
  page +=headerIndex;
  page += _menuIndex;
  page += "<div>\n";
  page += "<h2>Firmware Upload</h2>";
  page += "</div>\n";
  page += "<div><br /><br /></div>\n";
  page += "<div>\n";
  page += "<h2>On-The-Fly Firmware Update</h2>";
  page += "<h4>The uploaded firmware will be automatically applied</h4>";
  page += "<form method='POST' action='/update' enctype='multipart/form-data' id='update_form'>\n";
  page += "<input type='file' name='update'>\n";
  page += "<input type='submit' value='Update'>\n";
  page += "</form>\n";
  page += "</div>\n";
  page += _footerIndex;
  send(200, "text/html", page);
}

void
ESPMoticsWebServer::on_filelist()
{
  String page;
  char buff[64];

  sendHeader("Connection", "close");
  page +=headerIndex;
  page += _menuIndex;
  page += "<div>\n";
  page += "<h2>File Upload</h2>";
  page += "<form method='POST' action='/upload' enctype='multipart/form-data' id='upload_form'>\n";
  page += "<input type='file' name='upload'>\n";
  page += "<input type='submit' value='Upload'>\n";
  page += "</form>\n";
  page += "</div>\n";
  page += "<div>\n"; 
  page += "<br /><h2>Files:</h2>\n";
  if (_spiffsinit == false)
    page += "<h2>An Error has occurred while mounting SPIFFS</h2>\n";
  else
    {
      page += "<table width='50%'>\n";
      File root = SPIFFS.open("/");
      File file = root.openNextFile();
      page += "<tr>\n";
      page += "<td>\n";
      page += "File";
      page += "</td>\n";
      page += "<td>\n";
      page += "Size";
      page += "</td>\n";
      page += "<td>\n";
      page += "Actions:";
      page += "</td>\n";
      page += "</tr>\n";
      while(file)
	    {
            page += "<tr>\n";
	        page += "<td>\n SPIFFS: ";
	        page += getfullpath(file.name());
	        page += "</td>\n";
            page += "<td>\n";
            memset(buff, 0, 64*sizeof(char));
            size_t fsize = file.size();
            char unit = ((fsize / 1024 / 1024) > 0 ? 'M' : (fsize / 1024 > 0 ? 'K' : ' '));
	        sprintf(buff, "%.2f%cb", (float)((fsize / 1024 / 1024) > 0 ? (fsize / 1024 / 1024) : (fsize / 1024) > 0 ? (fsize / 1024) : fsize), unit);
            page += buff;
	        page += "</td>\n";
            page += "<td>\n";
            page += "<input type=button onClick=\"location.href='/delete?ref=/files&delete=";
	    page += getfullpath(file.name());
            page += "'\" value='delete'>";
            page += "<input type=button onClick=\"location.href='/rename?ref=/files&src=";
	    page += getfullpath(file.name());
            page += "'\" value='rename'>";
            page += "<input type=button onClick=\"location.href='/download?ref=/files&file=";
	    page += getfullpath(file.name());
            page += "'\" value='download'>";
	        page += "</td>\n";
            page += "</tr>\n";
	        file = root.openNextFile();
	    }
      page += "</table>\n";
    }
  page += "</div>";
  page += "<br /><br />\n";
  page += "<table width='50%'>\n";
  page += "<tr><td>\n";
  page += "Storage Statistics</td></tr>\n";
  page += "<tr><td>\n";
  page += "Used ";
  memset(buff, 0, 64*sizeof(char));
  page += itoa(SPIFFS.usedBytes(), buff, 10);
  page += " on a total of ";
  memset(buff, 0, 64*sizeof(char));
  page += itoa(SPIFFS.totalBytes(), buff, 10);
  page += " Bytes\n";
  page += "<progress value=\"";
  memset(buff, 0, 64*sizeof(char));
  page += itoa(SPIFFS.usedBytes(), buff, 10);
  page += "\" max=\"";
  memset(buff, 0, 64*sizeof(char));
  page += itoa(SPIFFS.totalBytes(), buff, 10);
  page += "\">\n";
  page += "</td></tr>\n";
  page += "<tr>\n";
  page += "<td>\n";
  page += "<input type=button onClick=\"location.href='/formatSPIFFS'\" value='SPIFFS Format'>";
  page += "</td>\n";
  page += "</tr>\n";
  page += "</table>\n";
  page += _footerIndex;
  send(200, "text/html", page);
}

void
ESPMoticsWebServer::on_delete()
{
    String redir;
    String todel;

    sendHeader("Connection", "close");
    redir = arg("ref");
    todel = arg("delete");
    if (todel != "")
      SPIFFS.remove(todel);
    sendHeader("Location", (redir == "" ? "/" : redir),true);
    send(302, "text/plain", "");
}

void
ESPMoticsWebServer::on_rename()
{
  String redir;
  String src;
  String dst;
  String page;

  sendHeader("Connection", "close");
  redir = arg("ref");
  if (redir == "")
    {
      sendHeader("Location", "/",true);
      send(302, "text/plain", "");
      return ;
    }
  src = arg("src");
  dst = arg("dst");
  if (src == "")
    {
      sendHeader("Location", redir, true);
      send(302, "text/plain", "");
      return ;
    }
  if (dst == "")
  {
    sendHeader("Connection", "close");
    page += _headerIndex;
    page += _menuIndex;
    page += "<div>\n";
    page += "<form method='GET' action='/rename' enctype='multipart/form-data' id='rename_form'>\n";
    page += "<table width='50%'>\n";
    page += "<tr>\n";
    page += "<td><input type='hidden' name='ref' value=\"";
    page += redir;
    page += "\">\n";
    page += "<td><input type='hidden' name='src' value=\"";
    page += src;
    page += "\">\n";
    page += "<input type='text' name='dst' value=\"";
    page += src;
    page += "\"></td>\n";
    page += "<td><input type='submit' value='rename'>\n";
    page += "</td>\n</tr>\n</table>\n";
    page += "</form>\n";
    page += "</div>\n";
    page += _footerIndex;
    send(200, "text/html", page);
  }
  else
  {
    SPIFFS.rename(src, dst);
    sendHeader("Location", redir, true);
    send(302, "text/plain", "");
    return ;
  }
}

void
ESPMoticsWebServer::on_format()
{
    String confirm;

    sendHeader("Connection", "close");
    confirm = arg("confirm");
    if (confirm == "")
    {
      String page;

      sendHeader("Connection", "close");
      page += _headerIndex;
      page += _menuIndex;
      page += "<div>\n";
      page += "<h1> You are going to erase all Flash content. Are you sure ?</h1>";
      page += "<table width='20%'>\n";
      page += "<tr>\n";
      page += "<td>\n";
      page += "<input type=button onClick=\"location.href='/formatSPIFFS?confirm=yes'\" value='Yes'>";
      page += "</td>\n";
      page += "</tr>\n";
      page += "<tr>\n";
      page += "<td>\n";
      page += "<input type=button onClick=\"location.href='/files'\" value='No'>";
      page += "</td>\n";
      page += "</tr>\n";
      page += "</table>\n";
      page += "</div>\n";
      page += _footerIndex;
      send(200, "text/html", page);
      return ;
    }
    if (confirm == "yes")
      SPIFFS.format();
    sendHeader("Location", "/files",true);
    send(302, "text/plain", "");

}

void
ESPMoticsWebServer::on_download()
{
    String redir;
    String todl;

    redir = arg("ref");
    todl = arg("file");
    if (todl == "")
      {
        sendHeader("Location", (redir == "" ? "/" : redir),true);
        send(302, "text/plain", "");
        return ;
      }
    File f;
    f = SPIFFS.open(todl.c_str(), "r");
    if (!f)
      {
        sendHeader("Location", (redir == "" ? "/" : redir),true);
        send(302, "text/plain", "");
        return ;
      }
    char buffer[BUFFLEN];
    int  c = 0;
    size_t fileSize = f.size();
    String disp;
    char *p = strrchr(f.name(), '/');
    disp = "attachment; filename=\"";
    disp += (p ? p+1 : f.name());
    disp += "\"";
    sendHeader("Content-Disposition", disp);
    setContentLength(fileSize);
    send(200, "application/octet-stream", "");
    while ((c = f.readBytes(buffer, BUFFLEN)) > 0)
      sendContent_P(buffer, c);
    f.close();
}


void
ESPMoticsWebServer::on_fupdate()
{
    String fupdate;

    sendHeader("Connection", "close");
    fupdate = arg("fupdate");
    if (fupdate == "")
      {
        // should 404, but for now, 302
        sendHeader("Location", "/firmware",true);
        send(302, "text/plain", "");
      }
      else
      {
        // add here local file load and update
        File fw;
        fw = SPIFFS.open(fupdate.c_str(), "r");
        size_t fileSize = fw.size();
        if ((!fw) || !Update.begin(fileSize))
        {
          //start with max available size
          sendHeader("Connection", "close");
          send(200, "text/plain", "FAIL1");
          return ;
        }
        Update.writeStream(fw);
        if(Update.end())
        {
# ifdef DEBUG_SERIAL_ENABLE
          Serial.println("Successful update");  
# endif
          fw.close();
          sendHeader("Connection", "close");
          send(200, "text/plain", "OK");
          ESP.restart();
        }
        else
        {
# ifdef DEBUG_SERIAL_ENABLE
          Serial.println("Error Occurred: " + String(Update.getError()));
# endif
          sendHeader("Connection", "close");
          send(200, "text/plain", "FAIL");
          return;
        }
      }
}

void
ESPMoticsWebServer::on_uploadurl()
{
    sendHeader("Connection", "close");
    sendHeader("Location", "/files",true);
    send(302, "text/plain", "");
}

void
ESPMoticsWebServer::do_upload()
{
    HTTPUpload& upload = this->upload();

    if (upload.status == UPLOAD_FILE_START) {
# ifdef DEBUG_SERIAL_ENABLE
      Serial.printf("Upload: %s\n", upload.filename.c_str());
# endif
      String fwname;
      fwname = "/";
      fwname += upload.filename;
      if (!_fw)
        _fw = SPIFFS.open(fwname.c_str(), "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* Saving received firmware to flash */
      size_t w;
      uint8_t count;
      for(w = 0, count = 0; w < upload.currentSize && count < 3; w += _fw.write(upload.buf+w, upload.currentSize - w), count++)
        _fw.flush();
        if (w == 0)
            {
# ifdef DEBUG_SERIAL_ENABLE
              Serial.printf("Upload Error: %u, filesystem full ?\n", upload.totalSize);
# endif
              _fw.close();
            }
    } else if (upload.status == UPLOAD_FILE_END) {
# ifdef DEBUG_SERIAL_ENABLE
        Serial.printf("Upload Success: %u\n", upload.totalSize);
# endif
          _fw.close(); 
      }
}

void
ESPMoticsWebServer::on_updateurl()
{
    sendHeader("Connection", "close");
    send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
}

void
ESPMoticsWebServer::do_update()
{
    HTTPUpload& upload = this->upload();

    if (upload.status == UPLOAD_FILE_START) {
# ifdef DEBUG_SERIAL_ENABLE
      Serial.printf("Update: %s\n", upload.filename.c_str());
# endif
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
# ifdef DEBUG_SERIAL_ENABLE
        Update.printError(Serial);
# endif
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
# ifdef DEBUG_SERIAL_ENABLE
        Update.printError(Serial);
# endif
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
# ifdef DEBUG_SERIAL_ENABLE
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
# endif
      } else {
# ifdef DEBUG_SERIAL_ENABLE
        Update.printError(Serial);
# endif
      }
    }
}

void
ESPMoticsWebServer::bindings()
{
  on("/", HTTP_GET, []() {
    if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_webroot();
  });
  on("/firmware", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_updatemenu();
  });
  on("/files", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_filelist();
  });
  on("/delete", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_delete();
  });
  on("/rename", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_rename();
  });
  on("/formatSPIFFS", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_format();
  });
  on("/download", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_download();
  });
  on("/fupdate", HTTP_GET, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_fupdate();
  });
  /*handling uploading firmware file */
  on("/upload", HTTP_POST, []() {
   if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
    espweb->requestAuthentication();
      espweb->on_uploadurl();
  }, []() {
      espweb->do_upload();
  });
  /*handling update firmware */
  on("/update", HTTP_POST, []() {
    if(!espweb->authenticate(espweb->getuser(), espweb->getpass()))
      espweb->requestAuthentication();
    espweb->on_updateurl();
  }, []() {
    espweb->do_update();
  });
  begin();
}

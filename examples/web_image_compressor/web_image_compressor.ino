#include <WiFi.h>
#include <ESPAsyncWebSrv.h> // ESPAsyncWebSrv by dvarrel
#include <Group5.h>

const String default_ssid = "your_ssid";
const String default_wifipassword = "your_password";
const int default_webserverporthttp = 80;
int iWidth, iHeight, iG5Size;
bool bHexOutput;
uint8_t *pBuffer, *pOut;
char szLeafName[128];
int iFileSize;
G5ENCODER g5enc;
BB_BITMAP bbbm;
int bin2c(char *leafname, uint8_t *pSrc, int iSrcSize, int w, int h);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
</head>
<body>
  <p><h1>ESP32 BMP File Compress Utility</h1></p>
  <p>Upload a 1-bit per pixel Windows BMP file and this utility will compress it into a Group5 compressed bitmap file</p>
  <form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/><input type="submit" name="upload" value="Upload" title="Upload File"></form>
  <form method="POST" action="/hex">Press this button to change the output file type: <input type="submit" name="hex_toggle" value="Toggle Hex Output"></form>
  <p>Download type: %TYPE%</p>
  <p>Image Dimensions: %IMAGESIZE% | Compressed Size: %G5SIZE%</p>
  <p>After clicking upload it will take some time for the file to upload,<br>
  there is no indicator that the upload began.  Please be patient.</p>
  <p>Once uploaded the page will refresh and image information will be displayed.</p>
  <p>After uploading, press the download link to retrieve the compressed file.</p>
  <p>%DOWNLOAD%</p>
  </body>
</html>
)rawliteral";

// configuration structure
struct Config {
  String ssid;               // wifi ssid
  String wifipassword;       // wifi password
  int webserverporthttp;     // http port number for web admin
};

// variables
Config config;                        // configuration
AsyncWebServer *server;               // initialise webserver

//
// Trim off the leaf name from a fully
// formed file pathname
//
void GetLeafName(char *fname, char *leaf)
{
    int i, iLen;
    
    iLen = (int)strlen(fname);
    for (i=iLen-1; i>=0; i--)
    {
        if (fname[i] == '\\' || fname[i] == '/') // Windows or Linux
            break;
    }
    strcpy(leaf, &fname[i+1]);
    // remove the filename extension
    iLen = (int)strlen(leaf);
    for (i=iLen-1; i>=0; i--)
    {
        if (leaf[i] == '.')
        {
            leaf[i] = 0;
            break;
        }
    }
} /* GetLeafName() */

uint32_t UnalignedRead32(uint8_t *s)
{
  uint32_t u32;
  u32 = (uint32_t)s[0];
  u32 += (uint32_t)s[1]<<8;
  u32 += (uint32_t)s[2]<<16;
  u32 += (uint32_t)s[3]<<24;
  return u32;
}
uint16_t UnalignedRead16(uint8_t *s)
{
  uint16_t u16;
  u16 = s[0];
  u16 += ((uint16_t)s[1]<<8);
  return u16;
}
// Read the file info and compress it with G5
void ProcessFile(void)
{
int y, rc, ibpp, offset, bytewidth, iPitch, iDelta;
uint8_t *s;

    if (pBuffer[0] != 'B' || pBuffer[1] != 'M' || pBuffer[14] < 0x28) {
        Serial.println("Not a Windows BMP file!");
        return;
    } else {
      Serial.println("Valid WinBMP!");
    }
    iWidth = UnalignedRead32(&pBuffer[18]);
    iHeight = UnalignedRead32(&pBuffer[22]);
    ibpp = UnalignedRead16(&pBuffer[26]) * UnalignedRead16(&pBuffer[28]);
    if (ibpp != 1) {
      Serial.println("Not a 1-bpp Bitmap!");
      return;
    } else {
      Serial.printf("%d x %d, %d bpp\n", iWidth, iHeight, ibpp);
    }
    offset = UnalignedRead32(&pBuffer[10]); // offset to bits
    Serial.printf("offset to bits: %d\n", offset);
    bytewidth = (iWidth+7) >> 3;
    iPitch = (bytewidth + 3) & 0xfffc; // DWORD aligned
    s = &pBuffer[offset];
    iDelta = iPitch;
    if (iHeight > 0) {
        iDelta = -iPitch;
        s = &pBuffer[offset + (iHeight-1) * iPitch];
    } else {
        iHeight = -iHeight;
    }
    Serial.printf("pitch = %d\n", iPitch);
    rc = g5enc.init(iWidth, iHeight, &pOut[sizeof(BB_BITMAP)], iPitch * iHeight);
    for (y=0; y<iHeight && rc == G5_SUCCESS; y++) {
        rc = g5enc.encodeLine(s);
        s += iDelta;
    } // for y
    if (rc == G5_ENCODE_COMPLETE) {
        Serial.println("G5 encode succeeded!");
        iG5Size = g5enc.size();
        Serial.printf("Input size:  %d bytes, output size: %d bytes\n", iPitch*iHeight, iG5Size);
        Serial.printf("Compression ratio: %2.1f:1\n", (float)(iPitch*iHeight) / (float)iG5Size);
        bbbm.u16Marker = BB_BITMAP_MARKER;
        bbbm.width = iWidth;
        bbbm.height = iHeight;
        bbbm.size = iG5Size;
        memcpy(pOut, &bbbm, sizeof(BB_BITMAP));
        iG5Size += sizeof(BB_BITMAP); // total file size
    }
    if (bHexOutput) { // turn it into hex data
       iG5Size = bin2c(szLeafName, pOut, iG5Size, iWidth, iHeight);
    }
} /* ProcessFile() */

void handleDownload(AsyncWebServerRequest *request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    Serial.println(logmessage);
    // See if the user wants the original binary file or a C hex version

  AsyncWebServerResponse *response = request->beginChunkedResponse("appliction/octet-stream", [](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
    if (iG5Size - alreadySent >= maxLen) {
      memcpy(buffer, &pOut[alreadySent], maxLen);
      return maxLen;
    } else {  // last chunk and then 0
      memcpy(buffer, &pOut[alreadySent], iG5Size - alreadySent);
      return iG5Size - alreadySent; 
    }
  });
  request->send(response);
} /* handleDownload() */

void setup() {
  iFileSize = 0; // no image loaded
  Serial.begin(115200);
  Serial.println("Booting ...");
  pBuffer = (uint8_t *)malloc(65536); // start with 64k
  pOut = (uint8_t *)malloc(16384); // start with 16k for output
  if (!pBuffer || !pOut) {
    Serial.println("malloc failed!");
    while (1) {};
  }
  Serial.println("\nLoading Configuration ...");

  config.ssid = default_ssid;
  config.wifipassword = default_wifipassword;
  config.webserverporthttp = default_webserverporthttp;

  Serial.print("\nConnecting to Wifi: ");
  WiFi.begin(config.ssid.c_str(), config.wifipassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n\nNetwork Configuration:");
  Serial.println("----------------------");
  Serial.print("         SSID: "); Serial.println(WiFi.SSID());
  Serial.print("  Wifi Status: "); Serial.println(WiFi.status());
  Serial.print("Wifi Strength: "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
  Serial.print("          MAC: "); Serial.println(WiFi.macAddress());
  Serial.print("           IP: "); Serial.println(WiFi.localIP());
  Serial.print("       Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("      Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("        DNS 1: "); Serial.println(WiFi.dnsIP(0));
  Serial.print("        DNS 2: "); Serial.println(WiFi.dnsIP(1));
  Serial.print("        DNS 3: "); Serial.println(WiFi.dnsIP(2));

  // configure web server
  Serial.println("\nConfiguring Webserver ...");
  server = new AsyncWebServer(config.webserverporthttp);
  configureWebServer();

  // startup web server
  Serial.println("Starting Webserver ...");
  server->begin();
}

void loop() {
}

void configureWebServer() {
  server->on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    Serial.println(logmessage);
    request->send_P(200, "text/html", index_html, processor);
  });

  // run handleDownload function when the output file is downloaded
  server->on("/download", HTTP_GET, handleDownload);
  server->on("/hex", HTTP_POST, handleHex);
  // run handleUpload function when any file is uploaded
  server->on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, handleUpload);
}

// handle hex toggle form
void handleHex(AsyncWebServerRequest *request) {
    bHexOutput = !bHexOutput; // toggle the setting
    request->redirect("/");
}
// handles uploads
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);

  //if (!index) {
  //  logmessage = "Upload Start: " + String(filename);
    // open the file on first call and store the file handle in the request object
  //  request->_tempFile = SPIFFS.open("/" + filename, "w");
  //  Serial.println(logmessage);
  //}
  iWidth = iHeight = iG5Size = 0; // same for these

  if (len) {
    Serial.printf("incoming file data size = %d\n", len);
    // stream the incoming chunk to the opened file
//    request->_tempFile.write(data, len);
    logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
    Serial.println(logmessage);
    memcpy(&pBuffer[index], data, len);
    iFileSize = index+len;
  }

  if (final) {
     GetLeafName((char *)filename.c_str(), szLeafName);
     Serial.print("Upload complete\n");
//    logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
    // close the file handle as the upload is now done
//    request->_tempFile.close();
//    Serial.println(logmessage);
    ProcessFile();
    request->redirect("/");
  }
}

String processor(const String& var) {
  char szTemp[256];
  char szFile[128];
  if (var == "DOWNLOAD") {
    if (iG5Size == 0) {
      strcpy(szTemp, "<p>Download link will appear here after uploading a valid BMP file</p>");
    } else {
      sprintf(szFile, "%s.%s", szLeafName, (bHexOutput) ? "h" : "g5");
      sprintf(szTemp, "<a href=\"/download\" download=\"%s\">Download %s</a>", szFile, szFile);
    }
    return String(szTemp);
  }
  if (var == "IMAGESIZE") {
    sprintf(szTemp, "%d x %d", iWidth, iHeight);
    return String(szTemp);
  }
  if (var == "TYPE") {
    if (bHexOutput) {
      return String("C hex values");
    } else {
      return String("G5 binary data");
    }
  }
  if (var == "G5SIZE") {
    sprintf(szTemp, "%d bytes", iG5Size);
    return String(szTemp);
  }

  return String();
}

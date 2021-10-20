/*
  This file is part of the XanderElectronics_WebUpdate library.
  Copyright (c) 2021 Xander Electronics. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <XanderElectronics_WebUpdate.h>

WiFiServer _server(80);

bool WebUpdateClass::begin(uint8_t csSD) {
  if (!SD.begin(csSD)) {
    Serial.println("SD not found");
    return false;
  }

  if (WiFi.status() == WL_NO_MODULE) {
    return false;
  }
  else {
    byte mac[6];
    WiFi.macAddress(mac);

    for (int i = 5; i >= 0; i--) {
      if (mac[i] < 16) {
        _macString += "0";
      }
      _macString += String(mac[i], HEX);
      if (i > 0) {
        _macString += ":";
      }
    }
    _macString.toUpperCase();

    int status;
    uint8_t tries = 0;

    do {
      status = WiFi.beginAP(_macString.c_str());
    } while (status != WL_AP_LISTENING && tries <= 3);

    if (status != WL_AP_LISTENING) {
      return false;
    }

    Serial.println("Scan for access point: " + _macString);
    Serial.println("Navigate to 192.168.4.1/update");

    delay(4000);
    _server.begin();
  }

  return true;
}

void WebUpdateClass::pollServer(void) {
  WiFiClient client = _server.available();
  pollServer(client);
}

String WebUpdateClass::getAPName(void) {
  return _macString;
}

//PRIVATE
void WebUpdateClass::pollServer(Client& client) {
  if (client) {

    while ((client.connected()) && (client.available())) {
      String request = client.readStringUntil('\n');
      request.trim();

      String header;
      long contentLength = -1;

      do {
        header = client.readStringUntil('\n');
        header.trim();

        if (header.startsWith(CONTENT_LENGTH)) {
          header.remove(0, strlen(CONTENT_LENGTH));
          contentLength = header.toInt();
        }
      } while (header != "");

      if (strstr(request.c_str(), "OPTIONS")) {
        _options =true;
        sendHttpResponse(client, 200, "OK", "OK");
      }

      else if (request == RGET_UPDATE) {
        client.flush();
        sendHttpResponse(client, 200, "OK", sizeof(AP_WEB_PAGE), HTTP_TYPE_HTML);
        for (uint32_t i = 0; i < sizeof(AP_WEB_PAGE); i++) {
          client.write(AP_WEB_PAGE[i]);
        }
        client.println();
        client.flush();
      }

      else if (request.startsWith(RPOST_UPDATE)) {
        saveFileFromClient(client, contentLength, String("UPDATE.BIN"), BODY_TYPE_OCTET);
        sendHttpResponse(client, 200, "OK", "OK");
        delay(1000);
        NVIC_SystemReset();
      }

      else {
        long readContentLength = readRequestBody(client, contentLength);
        sendHttpResponse(client, 404, "Not Found", "NOT FOUND");
      }

      delay(1000);
    }
    client.stop();
  }
}

void WebUpdateClass::sendCORSHeaders(Client & client) {
  client.println(CORS_ORIGIN);
  client.println(CORS_METHODS);
  client.println(CORS_HEADERS);
  client.println(CORS_AGE);
}

void WebUpdateClass::sendHttpResponse(Client & client, int code, const char* status) {
  String response = String(HTTP_TYPE) + " " + String(code) + " " + String(status);
  client.println(response);
  sendCORSHeaders(client);
  client.println();
}

void WebUpdateClass::sendHttpResponse(Client & client, int code, const char* status, unsigned long len, httpResponseType thisType) {
  String response = String(HTTP_TYPE) + " " + String(code) + " " + String(status);
  client.println(response);
  client.print(F("Content-Length: "));
  client.println(len);
  switch (thisType) {
    case HTTP_TYPE_JSON: client.println(APPLICATION_JSON_STRING); break;
    case HTTP_TYPE_CSS: client.println(TEXT_CSS_STRING); break;
    case HTTP_TYPE_JS: client.println(TEXT_JS_STRING); break;
    case HTTP_TYPE_PLAIN: client.println(TEXT_HTML_PLAIN_STRING); break;
    default: client.println(TEXT_HTML_STRING); break;
  }
  sendCORSHeaders(client);
  client.println();
}

void WebUpdateClass::sendHttpResponse(Client & client, int code, const char* status, String body) {
  String response = String(HTTP_TYPE) + " " + String(code) + " " + String(status);
  body.trim();
  client.println(response);
  client.print(F("Content-Length: "));
  client.println(body.length());
  client.println(TEXT_HTML_STRING);
  sendCORSHeaders(client);
  client.println();
  client.println(body);
  client.println();
}

long WebUpdateClass::forceFlush(Client & client, long requestLength) {
  long read = 0;
  while (client.connected() && read < requestLength) {
    if (client.available()) {
      char c = client.read();
      read++;
    }
  }
  return read;
}

long WebUpdateClass::readRequestBody(Client & client, long requestLength) {
  long read = 0;
  if (client.available()  > 0) {
    while (client.connected() && read < requestLength) {
      if (client.available()) {
        char c = client.read();
        read++;
      }
    }
    return read;
  }
  else {
    return -1;
  }
}

long WebUpdateClass::readRequestBodyUntil(Client& client, long requestLength, String matchString) {
  long read = 0;
  String header;

  while (client.connected() && read < requestLength) {
    if (client.available()) {
      do {
        header = client.readStringUntil('\n');
        read += header.length();
        header.trim();
      } while (header != matchString);
      return read;
    }
  }
  return read;
}

void WebUpdateClass::saveFileFromClient(Client& client, long contentLength, String thisFileName, bodyType thisBodyType) {
  long remainingLength = contentLength;

  removeIfExist(thisFileName.c_str());

  if (thisBodyType == BODY_TYPE_OCTET) {
    if (_options) {
      _options =false;
    }
    else {
      remainingLength -= readRequestBodyUntil(client, contentLength, OCTET_STREAM_STRING) - 2;
      forceFlush(client, 2); //there are two extra \n\r in the request
    }

    long savedChars = saveRequestBody(client, remainingLength, thisFileName);
  }
  else if (thisBodyType == BODY_TYPE_JSON) {
    readRequestBodyUntil(client, contentLength, APPLICATION_JSON_STRING);
    forceFlush(client, 2); //there are two extra \n\r in the request
    saveRequestBody(client, contentLength, thisFileName);
  }

  client.flush();
}

bool WebUpdateClass::removeIfExist(const char* thisFileName) {
  if (SD.exists(thisFileName)) {
    SD.remove(thisFileName);
    return true;
  }
  else {
    return false;
  }
}

long WebUpdateClass::saveRequestBody(Client & client, long requestLength, String filename) {
  long read = 0;

  if (client.available()) {
    File updateFile = SD.open(filename, FILE_WRITE);
    if (updateFile) {
      while (client.available()) {
        char c = client.read();
        read++;
        if (!_options) {
          if (c == '\r') {
            char c1 = client.read();
            read++;
            if (c1 == '\n') {
              char c2 = client.read();
              read++;
              if (c2 == '-') {
                break;
              }
              else {
                updateFile.write(c);
                updateFile.write(c1);
                updateFile.write(c2);
              }
            }
            else {
              updateFile.write(c);
              updateFile.write(c1);
            }
          }
          else {
            updateFile.write(c);
          }
        }
        else {
          updateFile.write(c);
        }
      }
      updateFile.close();
      return read;
    }
    else {
      return -1;
    }
  }
  else {
    return -1;
  }
}

WebUpdateClass WebUpdate;

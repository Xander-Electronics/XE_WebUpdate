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

#ifndef XANDERELECTRONICS_WEBUPDATE
#define XANDERELECTRONICS_WEBUPDATE

#include <Arduino.h>
#include <SDU.h>
#include <SPI.h>
#include <SD.h>
#include <WiFiNINA.h>
#include "webpage.h"
#include "requests.h"

class WebUpdateClass {
public:
  bool begin(uint8_t csSD = 10);
  void pollServer(void);
  String getAPName(void);

private:
  bool _options = false;
  String _macString = "XE_";

  void pollServer(Client& client);
  void sendCORSHeaders(Client & client);
  void sendHttpResponse(Client & client, int code, const char* status);
  void sendHttpResponse(Client & client, int code, const char* status, unsigned long len, httpResponseType thisType);
  void sendHttpResponse(Client & client, int code, const char* status, String body);
  long forceFlush(Client & client, long requestLength);
  long readRequestBody(Client & client, long requestLength);
  long readRequestBodyUntil(Client& client, long requestLength, String matchString);
  void saveFileFromClient(Client& client, long contentLength, String thisFileName, bodyType thisBodyType);
  bool removeIfExist(const char* thisFileName);
  long saveRequestBody(Client & client, long requestLength, String filename);

};

extern WebUpdateClass WebUpdate;

#endif

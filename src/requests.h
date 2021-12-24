/*
  This file is part of the XE_WebUpdate library.
  Copyright (c) 2021-2022 Xander Electronics. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef REQUESTS_H
#define REQUESTS_H

enum httpResponseType {HTTP_TYPE_HTML = 1, HTTP_TYPE_CSS, HTTP_TYPE_JS, HTTP_TYPE_JSON, HTTP_TYPE_PLAIN};
enum bodyType {BODY_TYPE_HTML = 1, BODY_TYPE_OCTET, BODY_TYPE_JSON};

#define HTTP_TYPE "HTTP/1.1"
#define CONTENT_LENGTH "Content-Length: "

#define RGET_UPDATE "GET /update HTTP/1.1"
#define RPOST_UPDATE "POST /update"

#define CORS_ORIGIN               "Access-Control-Allow-Origin: *"
#define CORS_METHODS              "Access-Control-Allow-Methods: POST, GET, PUT, DELETE"
#define CORS_HEADERS              "Access-Control-Allow-Headers: X-PINGOTHER, Content-Type, X-Requested-With"
#define CORS_AGE                  "Access-Control-Max-Age: 86400"

#define FORM_DATA_STRING          "Content-Disposition: form-data; name=\"filesize\""
#define OCTET_STREAM_STRING       "Content-Type: application/octet-stream"
#define APPLICATION_JSON_STRING   "Content-Type: application/json"
#define TEXT_CSS_STRING           "Content-Type: text/css"
#define TEXT_JS_STRING            "Content-Type: text/javascript"
#define TEXT_HTML_STRING          "Content-Type: text/html"
#define TEXT_HTML_PLAIN_STRING    "Content-Type: text/plain"

#endif //REQUESTS_H

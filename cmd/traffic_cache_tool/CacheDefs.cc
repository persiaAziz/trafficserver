/** @file

    Main program file for Cache Tool.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one
    or more contributor license agreements.  See the NOTICE file
    distributed with this work for additional information
    regarding copyright ownership.  The ASF licenses this file
    to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance
    with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include "CacheDefs.h"
#include <iostream>
using namespace std;
using namespace ts;

using ts::Errata;
namespace ts
{
std::ostream &
operator<<(std::ostream &s, Bytes const &n)
{
  return s << n.count() << " bytes";
}
std::ostream &
operator<<(std::ostream &s, Kilobytes const &n)
{
  return s << n.count() << " KB";
}
std::ostream &
operator<<(std::ostream &s, Megabytes const &n)
{
  return s << n.count() << " MB";
}
std::ostream &
operator<<(std::ostream &s, Gigabytes const &n)
{
  return s << n.count() << " GB";
}
std::ostream &
operator<<(std::ostream &s, Terabytes const &n)
{
  return s << n.count() << " TB";
}

std::ostream &
operator<<(std::ostream &s, CacheStripeBlocks const &n)
{
  return s << n.count() << " stripe blocks";
}
std::ostream &
operator<<(std::ostream &s, CacheStoreBlocks const &n)
{
  return s << n.count() << " store blocks";
}
std::ostream &
operator<<(std::ostream &s, CacheDataBlocks const &n)
{
  return s << n.count() << " data blocks";
}

Errata
URLparser::parseURL(StringView URI)
{
  Errata zret;
  static const StringView HTTP("http");
  static const StringView HTTPS("https");
  StringView scheme = URI.splitPrefix(':');
  if ((strcasecmp(scheme, HTTP) == 0) || (strcasecmp(scheme, HTTPS) == 0)) {
    StringView hostname = URI.splitPrefix(':');
    if (!hostname) // i.e. port not present
    {
    }
  }

  return zret;
}

int
URLparser::getPort(std::string &fullURL, int &port_ptr, int &port_len)
{
  url_matcher matcher;
  int n_port = -1;
  int u_pos  = -1;

  if (fullURL.find("https") == 0) {
    u_pos  = 8;
    n_port = 443;
  } else if (fullURL.find("http") == 0) {
    u_pos  = 7;
    n_port = 80;
  }
  if (u_pos != -1) {
    fullURL.insert(u_pos, ":@");
    static const StringView HTTP("http");
    static const StringView HTTPS("https");
    StringView url(fullURL.data(), (int)fullURL.size());

    url += 9;

    StringView hostPort = url.splitPrefix(':');
    if (hostPort) // i.e. port is present
    {
      StringView port = url.splitPrefix('/');
      if (!port) // i.e. backslash is not present, then the rest of the url must be just port
        port = url;
      if (matcher.portmatch(port.begin(), port.size())) {
        StringView text;
        n_port = svtoi(port, &text);
        if (text == port) {
          port_ptr = fullURL.find(':', 9);
          port_len = port.size();
          return n_port;
        }
      }
    }
    return n_port;
  } else {
    std::cout << "No scheme provided for: " << fullURL << std::endl;
    return -1;
  }
}
}
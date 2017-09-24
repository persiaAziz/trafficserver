/** @file

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

#include "LuaSNIConfig.h"
#include <cstring>

TsConfigDescriptor LuaSNIConfig::Item::FQDN_DESCRIPTOR{TsConfigDescriptor::Type::STRING, "String", "fqdn",
                                                       "Fully Qualified Domain Name"};
char LuaString[] = "sni_config = {\
{ fqdn:one.com, action:TLS.ACTION.TUNNEL, upstream_cert_verification:TLS.VERIFY.REQUIRED}\
}";

ts::Errata
LuaSNIConfig::loader(lua_State *s)
{
  char buff[256];
  int error;
  lua_State *L = lua_open(); /* opens Lua */
  // luaopen_base(L);             /* opens the basic library */
  // luaopen_table(L);            /* opens the table library */
  // luaopen_io(L);               /* opens the I/O library */
  // luaopen_string(L);           /* opens the string lib. */
  // luaopen_math(L);             /* opens the math lib. */
  luaL_loadbuffer(L, LuaString, strlen(LuaString), "LuaString");
}

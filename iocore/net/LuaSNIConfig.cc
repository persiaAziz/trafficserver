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
#include "ts/Diags.h"

TsConfigDescriptor LuaSNIConfig::desc = {TsConfigDescriptor::Type::ARRAY, "Array", "Item vector", "Vector"};
TsConfigArrayDescriptor LuaSNIConfig::DESCRIPTOR(LuaSNIConfig::desc);
TsConfigDescriptor LuaSNIConfig::Item::FQDN_DESCRIPTOR = {TsConfigDescriptor::Type::STRING, "String", "fqdn",
                                                          "Fully Qualified Domain Name"};

ts::Errata
LuaSNIConfig::loader(lua_State *L)
{
  ts::Errata zret;
  char buff[256];
  int error;

  lua_getfield(L,LUA_GLOBALSINDEX,"sni_config");
  int l_type = lua_type(L, -1);
  switch(l_type)
  {
      case LUA_TTABLE:
          Debug("ssl","table==============");
          break;
      case LUA_TSTRING:
          Debug("ssl","string============== %s",lua_tostring(L,-1));
          break;
      default:
          Debug("ssl","nothing===================");

  }
  if (l_type == LUA_TTABLE)
    Debug("ssl", "found table");
  else {
    zret.push(ts::Errata::Message(0, 0, "Invalid Lua Stack"));
  }
  return zret;
}

ts::Errata
LuaSNIConfig::Item::loader(lua_State *s)
{
  ts::Errata zret;
  return zret;
}

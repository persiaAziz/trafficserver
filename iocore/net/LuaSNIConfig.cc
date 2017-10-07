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

TsConfigEnumDescriptor LuaSNIConfig::Item::ACTION_DESCRIPTOR{
  TsConfigDescriptor::Type::ENUM, "enum", "Action", "Action for rule",
  {{"NONE", 0},{"TUNNEL", 1},{"MAP", 2}}
};

ts::Errata
LuaSNIConfig::loader(lua_State *L)
{
  ts::Errata zret;
//  char buff[256];
//  int error;

  lua_getfield(L, LUA_GLOBALSINDEX, "sni_config");
  int l_type = lua_type(L, -1);

  switch (l_type) {
  case LUA_TTABLE: // this has to be a multidimensional table
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
      l_type = lua_type(L, -1);
      if (l_type == LUA_TTABLE) { // the item should be table
        // new Item
        LuaSNIConfig::Item item;
        item.loader(L);
      } else {
        zret.push(ts::Errata::Message(0, 0, "Invalid Entry at SNI config"));
      }
      lua_pop(L, 1);
    }
    break;
  case LUA_TSTRING:
    Debug("ssl", "string value %s", lua_tostring(L, -1));
    break;
  default:
    zret.push(ts::Errata::Message(0, 0, "Invalid Lua SNI Config"));
    Debug("ssl", "Please check your SNI config");
    break;
  }

  return zret;
}

ts::Errata
LuaSNIConfig::Item::loader(lua_State *L)
{
  ts::Errata zret;
  int l_type = lua_type(L, -1);//-1 will contain the subarray now (since it is a value in the main table))
  lua_pushnil(L);
  while (lua_next(L, -2))
  {
    if(lua_type(L,-2)!=LUA_TSTRING)
    {
        Debug("ssl","string keys expected for entries in ssl_SNI.config");
    }
    const char *name = lua_tostring(L, -2);
    if(strncmp(name,TS_fqdn,strlen(TS_fqdn)))
    {
        FQDN_CONFIG.loader(L);
    }
    l_type = lua_type(L, -1);
    if (l_type == LUA_TSTRING) {
      Debug("ssl", "Entry name: %s value: %s _________________", name, lua_tostring(L, -1));
    } else if (l_type == LUA_TNUMBER) {
      Debug("ssl", "Entry name: %s value: %g ^^^^^^^^^^^^^^^^", name, lua_tonumber(L, -1));
    } else {
      zret.push(ts::Errata::Message(0, 0, "Invalid Entry at SNI config"));
    }
    lua_pop(L, 1);
  }
  return zret;
}

ts::Errata
LuaSNIConfig::registerEnum(lua_State* L)
{
    lua_newtable (L);
    lua_setglobal(L,"ActionTable");
    int i=start;
    LUA_ENUM(L,"disable_h2",i++);
    LUA_ENUM(L,"verify_client",i++);
    LUA_ENUM(L,"tunnel_route",i++);
    LUA_ENUM(L,"verify_origin_server",i++);
    LUA_ENUM(L,"client_cert",i++);
    return {};
}

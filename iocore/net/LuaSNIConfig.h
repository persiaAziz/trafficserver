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
/*
 * File:   LuaSNIConfig.h
 * Author: persia
 *
 * Created on September 21, 2017, 4:25 PM
 */

#ifndef LUASNICONFIG_H
#define LUASNICONFIG_H

#include "tsconfig/TsConfigLua.h"

#include <vector>

using ts::Errata;

struct LuaSNIConfig : public TsConfigBase {
  using self = LuaSNIConfig;
  enum class Action {
    TS_DISABLE_H2 = 0,
    TS_VERIFY_CLIENT, // this applies to server side vc only
    TS_TUNNEL_ROUTE,  // blind tunnel action
    TS_VERIFY_SERVER,
    TS_CLIENT_CERT
  };
  static TsConfigDescriptor desc;
  static TsConfigArrayDescriptor DESCRIPTOR;

  LuaSNIConfig() : TsConfigBase(this->DESCRIPTOR) { self::Item::Initialize(); }

  struct Item : public TsConfigBase {
    Item() : TsConfigBase(DESCRIPTOR), FQDN_CONFIG(FQDN_DESCRIPTOR, fqdn), ACTION_CONFIG(ACTION_DESCRIPTOR, action) {}
    ts::Errata loader(lua_State *s) override;
    static void
    Initialize()
    {
      ACTION_DESCRIPTOR.values = {
        {"TS_DISABLE_H2", 0}, {"TS_VERIFY_CLIENT", 1}, {"TS_TUNNEL_ROUTE", 2}, {"TS_VERIFY_SERVER", 3}, {"TS_CLIENT_CERT", 4}};

      ACTION_DESCRIPTOR.keys = {
        {0, "TS_DISABLE_H2"}, {1, "TS_VERIFY_CLIENT"}, {2, "TS_TUNNEL_ROUTE"}, {3, "TS_VERIFY_SERVER"}, {4, "TS_CLIENT_CERT"}};
    }

    std::string fqdn;
    int level;
    Action action;

    // These need to be initialized statically.
    static TsConfigObjectDescriptor OBJ_DESCRIPTOR;
    static TsConfigDescriptor FQDN_DESCRIPTOR;
    TsConfigString FQDN_CONFIG;
    // static TsConfigDescriptor LEVEL_DESCRIPTOR;
    // static TsConfigInt<Item> LEVEL_CONFIG;
    static TsConfigEnumDescriptor ACTION_DESCRIPTOR;
    TsConfigEnum<self::Action> ACTION_CONFIG;
    ~Item() {}
  };
  std::vector<self::Item> items;
  ts::Errata loader(lua_State *s) override;
};
#endif /* LUASNICONFIG_H */

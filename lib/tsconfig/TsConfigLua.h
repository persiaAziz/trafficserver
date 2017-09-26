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
 * File:   TSConfigLua.h
 * Author: persia
 *
 * Created on September 21, 2017, 4:04 PM
 */


#ifndef TSCONFIGLUA_H
#define TSCONFIGLUA_H


#include "tsconfig/Errata.h"
#include <unordered_map>
#include "luajit/src/lua.hpp"
#include <iostream>
/** Static schema data for a configuration value.

    This is a base class for data about a configuration value. This is intended to be a singleton
    static instance that contains schema data that is the same for all instances of the
    configuration value.
*/
struct TsConfigDescriptor {
  /// Type of the configuration value.
  enum class Type {
    ARRAY, ///< A homogenous array of nested values.
    OBJECT, ///< A set of fields, each a name / value pair.
    INT, ///< Integer value.
    FLOAT, ///< Floating point value.
    STRING, ///< String.
    ENUM ///< Enumeration (specialized).
  };
/*  TsConfigDescriptor() : type_name(nullptr),name(nullptr),description(nullptr) {}
  TsConfigDescriptor(Type typ,std::initializer_list<std::string> str_list): type(typ)
  {
      for (auto str :str_list) {
                std::cout << str << std::endl;
            }
  }
 * */
  Type type; ///< Value type.
  std::string type_name; ///< Literal type name used in the schema.
  std::string name; ///< Name of the configuration value.
  std::string description; ///< Description of the  value.
};

/** Configuration item instance data.

    This is an abstract base class for data about an instance of the value in a configuration
    struct. Actual instances will be a subclass for a supported configuration item type. This holds
    data that is per instance and therefore must be dynamically constructed as part of the
    configuration struct construction. The related description classes in contrast are data that is
    schema based and therefore can be static and shared among instances of the configuration struct.
*/
class TsConfigBase {
public:
  /// Source of the value in the config struct.
  enum class Source {
    NONE, ///< No source, the value is default constructed.
    SCHEMA, ///< Value set in schema.
    CONFIG ///< Value set in configuration file.
  };
  /// Constructor - need the static descriptor.
  TsConfigBase(TsConfigDescriptor const& d) : descriptor(d) {}
  TsConfigDescriptor const& descriptor; ///< Static schema data.
  Source source = Source::NONE; ///< Where the instance data came from.
  ~TsConfigBase()
  {}
  /// Load the instance data from the Lua stack.
  virtual ts::Errata loader(lua_State* s) = 0;
};

class TsConfigInt : public TsConfigBase {
public:
   TsConfigInt(TsConfigDescriptor const& d, int& i);
   int & ref;
   ts::Errata loader(lua_State* s) override;
};

class TsConfigString : public TsConfigBase {
public:
   TsConfigString(TsConfigDescriptor const& d, std::string& str) : TsConfigBase(d), ref(str) {}
    std::string& ref;
    TsConfigString& operator= (const TsConfigString& other)
    {
        ref = other.ref;
        return *this;
    }
   ts::Errata loader(lua_State* s) override;
};

template < typename E >
class TsConfigEnum : public TsConfigBase {
public:
   TsConfigEnum(TsConfigDescriptor const& d, E& i) : TsConfigBase(d), ref(i) {}
   E& ref;
   TsConfigEnum& operator= (const TsConfigEnum& other)
    {
        ref = other.ref;
        return *this;
    }
   ts::Errata loader(lua_State* s) override
   {
    ts::Errata zret;
    return zret;
    }
};

class TsConfigArrayDescriptor : public TsConfigDescriptor {
public:
   TsConfigArrayDescriptor(TsConfigDescriptor const& d) : item(d) {}
   const TsConfigDescriptor& item;
};

class TsConfigEnumDescriptor : public TsConfigDescriptor {
   std::unordered_map<std::string, int> values;
   std::unordered_map<int, std::string> keys;
};

class TsConfigObjectDescriptor : public TsConfigDescriptor {
   std::unordered_map<std::string, TsConfigDescriptor const*> fields;
};

#endif /* TSCONFIGLUA_H */

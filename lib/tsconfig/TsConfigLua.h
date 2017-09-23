/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TSConfigLua.h
 * Author: persia
 *
 * Created on September 21, 2017, 4:04 PM
 */


#ifndef TSCONFIGLUA_H
#define TSCONFIGLUA_H


#include "Errata.h"
#include "lua.h"
#include <unordered_map>

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

  /// Load the instance data from the Lua stack.
  virtual ts::Errata loader(lua_State* s) = 0;
};

class TsConfigInt : public TsConfigBase {
   TsConfigInt(TsConfigDescriptor const& d, int& i) : TsConfigBase(d), ref(i) {}
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
   ts::Errata loader(lua_State* s) override;
};

class TsConfigArrayDescriptor : public TsConfigDescriptor {
public:
   TsConfigArrayDescriptor(TsConfigDescriptor const& d) : item(d) {}
   TsConfigDescriptor const& item;
};

class TsConfigEnumDescriptor : public TsConfigDescriptor {
   std::unordered_map<std::string, int> values;
   std::unordered_map<int, std::string> keys;
};

class TsConfigObjectDescriptor : public TsConfigDescriptor {
   std::unordered_map<std::string, TsConfigDescriptor const*> fields;
};

#endif /* TSCONFIGLUA_H */

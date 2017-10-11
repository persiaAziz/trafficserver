/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "TsConfigLua.h"

ts::Errata TsConfigInt::loader(lua_State* s)
{
    ts::Errata zret;
    ref = lua_tonumber(s,-1);
    return zret;
}

ts::Errata TsConfigString::loader(lua_State* s)
{
    ts::Errata zret;
    ref = lua_tostring(s,-1);
    return zret;

}

ts::Errata TsConfigBool::loader(lua_State* s)
{
    ts::Errata zret;
    ref = lua_toboolean(s,-1);
    return zret;
}

//template < typename E >
//ts::Errata TsConfigEnum<E>::loader(lua_State* s)
//{
//    ts::Errata zret;
//    return zret;
//}

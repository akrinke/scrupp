/*    Copyright (c) 2010 Andreas Krinke
 *    Copyright (c) 2009 Mr C.Camacho
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *    
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *    
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *    THE SOFTWARE.
 */ 

#include <stdlib.h>

#include "chipmunk.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpBody-lua.h"
#include "cpConstraint-lua.h"
#include "cpMisc-lua.h"

#define check_cpSimpleMotor(L, index) \
  *(cpSimpleMotor **)luaL_checkudata(L, (index), "cpSimpleMotor")

static cpSimpleMotor *push_cpSimpleMotor (lua_State *L) {
  cpSimpleMotor *sm = cpSimpleMotorAlloc();
  cpSimpleMotor **psm = (cpSimpleMotor **)lua_newuserdata(L, sizeof(cpSimpleMotor*));
  *psm = sm;

  luaL_getmetatable(L, "cpSimpleMotor");
  lua_setmetatable(L, -2);

  return sm;
}

static int cpSimpleMotor_new (lua_State *L) {
  cpBody* b1 = check_cpBody (L, 1);
  cpBody* b2 = check_cpBody (L, 2);
  cpFloat rate = (cpFloat)luaL_checknumber (L, 3);

  cpSimpleMotor *sm = push_cpSimpleMotor(L);
  cpSimpleMotorInit(sm, b1, b2, rate);

  cpConstraint_store_refs(L);
  return 1;
}

WCP_DefineConstraintGetterSetterFloat(SimpleMotor, Rate, rate);

static int cpSimpleMotor_tostring (lua_State *L) {
  lua_pushfstring(L, "cpSimpleMotor (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpSimpleMotor_functions[] = {
  {"newSimpleMotor", cpSimpleMotor_new},
  {NULL, NULL}
};

static const luaL_reg cpSimpleMotor_methods[] = {
  WCP_METHOD(SimpleMotor, getRate),
  WCP_METHOD(SimpleMotor, setRate),
  DEFINE_CONSTRAINT_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpSimpleMotor_meta[] = {
  {"__gc",       cpConstraint_gc},
  {"__tostring", cpSimpleMotor_tostring},
  {NULL, NULL}
};

int cpSimpleMotor_register (lua_State *L) {
  luaL_register(L, NULL, cpSimpleMotor_functions);

  luaL_newmetatable(L, "cpSimpleMotor");
  luaL_register(L, NULL, cpSimpleMotor_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpSimpleMotor_methods);
  lua_rawset(L, -3);
  
  /* cpConstraints.metatable = 1 */
  lua_pushliteral(L, "cpConstraints");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_pushinteger(L, 1);
  lua_rawset(L, -3);

  /* drop metatable and cpConstraints table */
  lua_pop(L, 2);

  return 0;
}

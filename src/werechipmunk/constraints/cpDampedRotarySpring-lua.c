/*    Copyright (c) 2010 Andreas Krinke
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
#include "cpVect-lua.h"

#define check_cpDampedRotarySpring(L, index) \
  *(cpDampedRotarySpring **)luaL_checkudata(L, (index), "cpDampedRotarySpring")

static cpDampedRotarySpring *push_cpDampedRotarySpring (lua_State *L) {
  cpDampedRotarySpring *drs = cpDampedRotarySpringAlloc();
  cpDampedRotarySpring **pdrs = (cpDampedRotarySpring **)lua_newuserdata(L, sizeof(cpDampedRotarySpring*));
  *pdrs = drs;

  luaL_getmetatable(L, "cpDampedRotarySpring");
  lua_setmetatable(L, -2);

  return drs;
}

static int cpDampedRotarySpring_new (lua_State *L) {
  cpBody *b1 = check_cpBody(L, 1);
  cpBody *b2 = check_cpBody(L, 2);
  cpFloat restAngle = luaL_checknumber(L, 3);
  cpFloat stiffness = luaL_checknumber(L, 4);
  cpFloat damping = luaL_checknumber(L, 5);

  cpDampedRotarySpring *drs = push_cpDampedRotarySpring(L);
  cpDampedRotarySpringInit(drs, b1, b2, restAngle, stiffness, damping);

  cpConstraint_store_refs(L);
  return 1;
}

WCP_DefineConstraintGetterSetterFloat(DampedRotarySpring, RestAngle, restAngle);
WCP_DefineConstraintGetterSetterFloat(DampedRotarySpring, Stiffness, stiffness);
WCP_DefineConstraintGetterSetterFloat(DampedRotarySpring, Damping, damping);

static int cpDampedRotarySpring_tostring (lua_State *L) {
  lua_pushfstring(L, "cpDampedRotarySpring (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpDampedRotarySpring_functions[] = {
  {"newDampedRotarySpring", cpDampedRotarySpring_new},
  {NULL, NULL}
};

static const luaL_reg cpDampedRotarySpring_methods[] = {
  WCP_METHOD(DampedRotarySpring, getRestAngle),
  WCP_METHOD(DampedRotarySpring, setRestAngle),
  WCP_METHOD(DampedRotarySpring, getStiffness),
  WCP_METHOD(DampedRotarySpring, setStiffness),
  WCP_METHOD(DampedRotarySpring, getDamping),
  WCP_METHOD(DampedRotarySpring, setDamping),
  DEFINE_CONSTRAINT_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpDampedRotarySpring_meta[] = {
  {"__gc",       cpConstraint_gc},
  {"__tostring", cpDampedRotarySpring_tostring},
  {NULL, NULL}
};

int cpDampedRotarySpring_register (lua_State *L) {
  luaL_register(L, NULL, cpDampedRotarySpring_functions);

  luaL_newmetatable(L, "cpDampedRotarySpring");
  luaL_register(L, NULL, cpDampedRotarySpring_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpDampedRotarySpring_methods);
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

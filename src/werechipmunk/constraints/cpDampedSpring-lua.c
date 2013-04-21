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

#define check_cpDampedSpring(L, index) \
  *(cpDampedSpring **)luaL_checkudata(L, (index), "cpDampedSpring")

static cpDampedSpring *push_cpDampedSpring (lua_State *L) {
  cpDampedSpring *ds = cpDampedSpringAlloc();
  cpDampedSpring **pds = (cpDampedSpring **)lua_newuserdata(L, sizeof(cpDampedSpring*));
  *pds = ds;

  luaL_getmetatable(L, "cpDampedSpring");
  lua_setmetatable(L, -2);

  return ds;
}

static int cpDampedSpring_new (lua_State *L) {
  cpBody *b1 = check_cpBody(L, 1);
  cpBody *b2 = check_cpBody(L, 2);
  cpVect a1 = check_cpVect(L, 3);
  cpVect a2 = check_cpVect(L, 5);
  cpFloat restLength = luaL_checknumber(L, 7);
  cpFloat stiffness = luaL_checknumber(L, 8);
  cpFloat damping = luaL_checknumber(L, 9);

  cpDampedSpring *ds = push_cpDampedSpring(L);
  cpDampedSpringInit(ds, b1, b2, a1, a2, restLength, stiffness, damping);

  cpConstraint_store_refs(L);
  return 1;
}

WCP_DefineConstraintGetterSetterVect(DampedSpring, Anchor1, anchr1);
WCP_DefineConstraintGetterSetterVect(DampedSpring, Anchor2, anchr2);
WCP_DefineConstraintGetterSetterFloat(DampedSpring, RestLength, restLength);
WCP_DefineConstraintGetterSetterFloat(DampedSpring, Stiffness, stiffness);
WCP_DefineConstraintGetterSetterFloat(DampedSpring, Damping, damping);

static int cpDampedSpring_tostring (lua_State *L) {
  lua_pushfstring(L, "cpDampedSpring (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpDampedSpring_functions[] = {
  {"newDampedSpring", cpDampedSpring_new},
  {NULL, NULL}
};

static const luaL_reg cpDampedSpring_methods[] = {
  WCP_METHOD(DampedSpring, getAnchor1),
  WCP_METHOD(DampedSpring, setAnchor1),
  WCP_METHOD(DampedSpring, getAnchor2),
  WCP_METHOD(DampedSpring, setAnchor2),
  WCP_METHOD(DampedSpring, getRestLength),
  WCP_METHOD(DampedSpring, setRestLength),
  WCP_METHOD(DampedSpring, getStiffness),
  WCP_METHOD(DampedSpring, setStiffness),
  WCP_METHOD(DampedSpring, getDamping),
  WCP_METHOD(DampedSpring, setDamping),
  DEFINE_CONSTRAINT_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpDampedSpring_meta[] = {
  {"__gc",       cpConstraint_gc},
  {"__tostring", cpDampedSpring_tostring},
  {NULL, NULL}
};

int cpDampedSpring_register (lua_State *L) {
  luaL_register(L, NULL, cpDampedSpring_functions);

  luaL_newmetatable(L, "cpDampedSpring");
  luaL_register(L, NULL, cpDampedSpring_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpDampedSpring_methods);
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

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

#include <chipmunk.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpVect-lua.h"

#define toarbiterp(L) \
  (cpArbiter **)luaL_checkudata(L, 1, "cpArbiter")

void push_cpArbiter(lua_State *L, cpArbiter *arb) {
  cpArbiter **ptr = (cpArbiter **)lua_newuserdata(L, sizeof(cpArbiter *));
  *ptr = arb;
  luaL_getmetatable(L, "cpArbiter");
  lua_setmetatable(L, -2);
}

static cpArbiter *toarbiter(lua_State *L) {
  cpArbiter **arb = toarbiterp(L);
  if (*arb == NULL) {
    luaL_error(L, "attempt to use arbiter outside of callback");
  }
  return *arb;
}

static int cpArbiter_getShapes(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  CP_ARBITER_GET_SHAPES(arb, a, b);
  
  lua_pushliteral(L, "cpShape_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, a);
  lua_rawget(L, -2);
  lua_pushlightuserdata(L, b);
  lua_rawget(L, -3);
  
  return 2;
}

static int cpArbiter_isFirstContact(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  lua_pushboolean(L, cpArbiterIsFirstContact(arb));
  return 1;
}

static int cpArbiter_getNormal(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  push_cpVect(L, cpArbiterGetNormal(arb, 0));
  return 2;
}

static int cpArbiter_getPoint(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  int i = luaL_checkint(L, 2);
  push_cpVect(L, cpArbiterGetPoint(arb, i));
  return 2;
}

static int cpArbiter_totalImpulse(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  push_cpVect(L, cpArbiterTotalImpulse(arb));
  return 2;
}

static int cpArbiter_totalImpulseWithFriction(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  push_cpVect(L, cpArbiterTotalImpulseWithFriction(arb));
  return 2;
}

static int cpArbiter_getE(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  lua_pushnumber(L, arb->e);
  return 1;
}

static int cpArbiter_setE(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  arb->e = luaL_checknumber(L, 2);
  return 0;
}

static int cpArbiter_getU(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  lua_pushnumber(L, arb->u);
  return 1;
}

static int cpArbiter_setU(lua_State *L) {
  cpArbiter *arb = toarbiter(L);
  arb->u = luaL_checknumber(L, 2);
  return 0;
}

static int cpArbiter_tostring(lua_State *L) {
  lua_pushfstring(L, "cpArbiter (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpArbiter_methods[] = {
  {"getShapes",                   cpArbiter_getShapes},
  {"isFirstContact",              cpArbiter_isFirstContact},
  {"getNormal",                   cpArbiter_getNormal},
  {"getPoint",                    cpArbiter_getPoint},
  {"getTotalImpulse",             cpArbiter_totalImpulse},
  {"getTotalImpulseWithFriction", cpArbiter_totalImpulseWithFriction},
  {NULL, NULL}
};

static const luaL_reg cpArbiter_meta[] = {
  {"__tostring", cpArbiter_tostring},
  {NULL, NULL}
};

int cpArbiter_register(lua_State *L) {
  luaL_newmetatable(L, "cpArbiter");
  /* metatable.__index = methods */
  lua_pushliteral(L, "__index");
  /* create and fill methods table */
  lua_newtable(L);
  luaL_register(L, NULL, cpArbiter_methods);
  lua_rawset(L, -3);
  /* pop the metatable */
  lua_pop(L, 1);
  return 0;
}

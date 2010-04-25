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

#define check_cpSlideJoint(L, index) \
  *(cpSlideJoint **)luaL_checkudata(L, (index), "cpSlideJoint")

static cpSlideJoint *push_cpSlideJoint (lua_State *L) {
  cpSlideJoint *sj = cpSlideJointAlloc();
  cpSlideJoint **psj = (cpSlideJoint **)lua_newuserdata(L, sizeof(cpSlideJoint *));
  *psj = sj;

  luaL_getmetatable(L, "cpSlideJoint");
  lua_setmetatable(L, -2);

  return sj;
}

static int cpSlideJoint_new (lua_State *L) {
  cpBody *b1 = check_cpBody(L, 1);
  cpBody *b2 = check_cpBody(L, 2);
  cpVect a1 = check_cpVect(L, 3);
  cpVect a2 = check_cpVect(L, 5);
  cpFloat min = luaL_checknumber(L, 7);
  cpFloat max = luaL_checknumber(L, 8);

  cpSlideJoint *sj = push_cpSlideJoint(L);
  cpSlideJointInit(sj, b1, b2, a1, a2, min, max);

  cpConstraint_store_refs(L);
  return 1;
}

WCP_DefineConstraintGetterSetterVect(SlideJoint, Anchor1, anchr1);
WCP_DefineConstraintGetterSetterVect(SlideJoint, Anchor2, anchr2);
WCP_DefineConstraintGetterSetterFloat(SlideJoint, Min, min);
WCP_DefineConstraintGetterSetterFloat(SlideJoint, Max, max);

static int cpSlideJoint_tostring (lua_State *L) {
  lua_pushfstring(L, "cpSlideJoint (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpSlideJoint_functions[] = {
  {"newSlideJoint", cpSlideJoint_new},
  {NULL, NULL}
};

static const luaL_reg cpSlideJoint_methods[] = {
  WCP_METHOD(SlideJoint, getAnchor1),
  WCP_METHOD(SlideJoint, setAnchor1),
  WCP_METHOD(SlideJoint, getAnchor2),
  WCP_METHOD(SlideJoint, setAnchor2),
  WCP_METHOD(SlideJoint, getMin),
  WCP_METHOD(SlideJoint, setMin),
  WCP_METHOD(SlideJoint, getMax),
  WCP_METHOD(SlideJoint, setMax),
  DEFINE_CONSTRAINT_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpSlideJoint_meta[] = {
  {"__gc",       cpConstraint_gc},
  {"__tostring", cpSlideJoint_tostring},
  {NULL, NULL}
};

int cpSlideJoint_register (lua_State *L) {
  luaL_register(L, NULL, cpSlideJoint_functions);

  luaL_newmetatable(L, "cpSlideJoint");
  luaL_register(L, NULL, cpSlideJoint_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpSlideJoint_methods);
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

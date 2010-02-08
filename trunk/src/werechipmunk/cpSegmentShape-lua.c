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
#include "cpShape-lua.h"
#include "cpVect-lua.h"

#define check_cpSegmentShape(L, index) \
  *(cpSegmentShape **)luaL_checkudata(L, (index), "cpSegmentShape")

static cpSegmentShape *push_cpSegmentShape (lua_State *L) {
  cpSegmentShape *ss = cpSegmentShapeAlloc();
  cpSegmentShape **pss = (cpSegmentShape**)lua_newuserdata(L, sizeof(cpSegmentShape *));
  *pss = ss;

  luaL_getmetatable(L, "cpSegmentShape");
  lua_setmetatable(L, -2);

  /* cpShape_ptrs.shape_ptr = shape_userdata */
  lua_pushliteral(L, "cpShape_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, ss);
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return ss;
}

static int cpSegmentShape_new (lua_State *L) {
  cpBody *body = check_cpBody(L, 1);
  cpVect a = check_cpVect(L, 2);
  cpVect b = check_cpVect(L, 4);
  cpFloat radius = (cpFloat)luaL_checknumber(L, 6);

  cpSegmentShape *ss = push_cpSegmentShape(L);
  cpSegmentShapeInit(ss, body, a, b, radius);
  
  /* cpReferences.shape_userdata = body_userdata */
  lua_pushliteral(L, "cpReferences");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_pushvalue(L, 1);
  lua_rawset(L, -3);
  lua_pop(L, 1);
  return 1;
}

static int cpSegmentShape_getA (lua_State *L) {
  cpSegmentShape *ss = check_cpSegmentShape(L, 1);
  push_cpVect(L, cpSegmentShapeGetA((cpShape*)ss));
  return 2;
}

static int cpSegmentShape_getB (lua_State *L) {
  cpSegmentShape *ss = check_cpSegmentShape(L, 1);
  push_cpVect(L, cpSegmentShapeGetB((cpShape*)ss));
  return 2;
}

static int cpSegmentShape_getNormal (lua_State *L) {
  cpSegmentShape *ss = check_cpSegmentShape(L, 1);
  push_cpVect(L, cpSegmentShapeGetNormal((cpShape*)ss));
  return 2;
}

static int cpSegmentShape_getRadius (lua_State *L) {
  cpSegmentShape *ss = check_cpSegmentShape(L, 1);
  lua_pushnumber(L, (double)cpSegmentShapeGetRadius((cpShape*)ss));
  return 1;
}

static int cpSegmentShape_tostring (lua_State *L) {
  lua_pushfstring(L, "cpSegmentShape (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpSegmentShape_functions[] = {
  {"newSegmentShape", cpSegmentShape_new},
  {NULL, NULL}
};

static const luaL_reg cpSegmentShape_methods[] = {
  {"getA",             cpSegmentShape_getA},
  {"getB",             cpSegmentShape_getB},
  {"getNormal",        cpSegmentShape_getNormal},
  {"getRadius",        cpSegmentShape_getRadius},
  DEFINE_SHAPE_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpSegmentShape_meta[] = {
  {"__gc",       cpShape_gc},
  {"__tostring", cpSegmentShape_tostring},
  {NULL, NULL}
};

int cpSegmentShape_register (lua_State *L) {
  luaL_register(L, NULL, cpSegmentShape_functions);
  
  luaL_newmetatable(L, "cpSegmentShape");
  luaL_register(L, NULL, cpSegmentShape_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpSegmentShape_methods);
  lua_rawset(L, -3);
  
  /* cpShapes.metatable = 1 */
  lua_pushliteral(L, "cpShapes");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_pushinteger(L, 1);  
  lua_rawset(L, -3);
  
  /* drop metatable and cpShapes table */
  lua_pop(L, 2);

  return 0;
}

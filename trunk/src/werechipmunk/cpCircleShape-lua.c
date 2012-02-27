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

#define check_cpCircleShape(L, index) \
  *(cpCircleShape **)luaL_checkudata(L, (index), "cpCircleShape")

static cpCircleShape *push_cpCircleShape (lua_State *L) {
  cpCircleShape *cs = cpCircleShapeAlloc();
  cpCircleShape **ptr = (cpCircleShape **)lua_newuserdata(L, sizeof(cpCircleShape*));
  *ptr = cs;

  luaL_getmetatable(L, "cpCircleShape");
  lua_setmetatable(L, -2);

  /* cpShape_ptrs.shape_ptr = shape_userdata */
  lua_pushliteral(L,"cpShape_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, cs);
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return cs;
}

static int cpCircleShape_new (lua_State *L) {
  cpBody *body = check_cpBody(L, 1);
  cpFloat radius = (cpFloat)luaL_checknumber(L, 2);
  cpVect offset = check_cpVect(L, 3);

  cpCircleShape *circle = push_cpCircleShape(L);
  cpCircleShapeInit(circle, body, radius, offset);

  /* cpReferences.shape_userdata = body_userdata */
  lua_pushliteral(L, "cpReferences");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_pushvalue(L, 1);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return 1;
}
 
static int cpCircleShape_getOffset (lua_State *L) {
  cpCircleShape *cs = check_cpCircleShape (L, 1);
  push_cpVect(L, cpCircleShapeGetOffset((cpShape*)cs));
  return 2;
}
 
static int cpCircleShape_getRadius (lua_State *L) {
  cpCircleShape* cs = check_cpCircleShape(L, 1);
  lua_pushnumber(L, cpCircleShapeGetRadius((cpShape*)cs));
  return 1;
}

static int cpCircleShape_tostring (lua_State *L) {
  lua_pushfstring(L, "cpCircleShape (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpCircleShape_functions[] = {
  {"newCircleShape", cpCircleShape_new},
  {NULL, NULL}
};

static const luaL_reg cpCircleShape_methods[] = {
  {"getOffset",        cpCircleShape_getOffset},
  {"getRadius",        cpCircleShape_getRadius},
  DEFINE_SHAPE_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpCircleShape_meta[] = {
  {"__gc",       cpShape_gc},
  {"__tostring", cpCircleShape_tostring},
  {NULL, NULL}
};

int cpCircleShape_register (lua_State *L) {
  luaL_register(L, NULL, cpCircleShape_functions);
  
  luaL_newmetatable(L, "cpCircleShape");
  luaL_register(L, NULL, cpCircleShape_meta);
  /* metatable.__index = methods */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpCircleShape_methods);
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

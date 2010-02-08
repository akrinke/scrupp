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

#define check_cpPolyShape(L, index) \
  *(cpPolyShape **)luaL_checkudata(L, (index), "cpPolyShape")

static cpPolyShape *push_cpPolyShape (lua_State *L) {
  cpPolyShape *ps = cpPolyShapeAlloc();
  cpPolyShape **psp = (cpPolyShape **)lua_newuserdata(L, sizeof(cpPolyShape *));
  *psp = ps;

  luaL_getmetatable(L, "cpPolyShape");
  lua_setmetatable(L, -2);

  /* cpShape_ptrs.shape_pointer = shape_userdata */
  lua_pushliteral(L, "cpShape_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, ps);
  /* duplicate userdata */
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return ps;
}

static int cpPolyShape_new (lua_State *L) {
  cpBody *body = check_cpBody(L, 1);
  luaL_checktype(L, 2, LUA_TTABLE);
  int n = lua_objlen(L, 2);
  luaL_argcheck(L, (n % 2 == 0) && (n > 4), 2, "at least 3 pairs of coordinates are requires");
  cpVect offset = check_cpVect(L, 3);

  cpVect *verts = (cpVect *)malloc(n/2 * sizeof(cpVect));
  cpVect *tv = verts;

  int i;
  for (i=1; i<n; i=i+2) {
    lua_pushinteger(L, i);
    lua_gettable(L, 2);
    lua_pushinteger(L, i+1);
    lua_gettable(L, 2);
    tv->x = (cpFloat)luaL_checknumber(L, -2);
    tv->y = (cpFloat)luaL_checknumber(L, -1);
    tv++;
    lua_pop(L, 2);
  }
  cpPolyShape *poly = push_cpPolyShape(L);
  cpPolyShapeInit(poly, body, n/2, verts, offset);
  free(verts);
  
  /* cpReferences.shape_userdata = body_userdata */
  lua_pushliteral(L, "cpReferences");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_pushvalue(L, 1);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return 1;
}

static int cpPolyShape_getVert (lua_State *L) {
  cpPolyShape *ps = check_cpPolyShape(L, 1);
  int i = luaL_checkint(L, 2);
  push_cpVect(L, cpPolyShapeGetVert((cpShape*)ps, i-1));
  return 2;
}

static int cpPolyShape_getNumVerts (lua_State *L) {
  cpPolyShape* ps = check_cpPolyShape(L, 1);
  lua_pushinteger(L, cpPolyShapeGetNumVerts((cpShape *)ps));
  return 1;
}

static int cpPolyShape_tostring (lua_State *L) {
  lua_pushfstring(L, "cpPolyShape (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpPolyShape_functions[] = {
  {"newPolyShape", cpPolyShape_new},
  {NULL, NULL}
};

static const luaL_reg cpPolyShape_methods[] = {
  {"getVert",          cpPolyShape_getVert},
  {"getNumVerts",      cpPolyShape_getNumVerts},
  DEFINE_SHAPE_METHODS,
  {NULL, NULL}
};

static const luaL_reg cpPolyShape_meta[] = {
  {"__gc",       cpShape_gc},
  {"__tostring", cpPolyShape_tostring},
  {NULL, NULL}
};

int cpPolyShape_register (lua_State *L) {
  luaL_register(L, NULL, cpPolyShape_functions);
  
  luaL_newmetatable(L, "cpPolyShape");
  luaL_register(L, NULL, cpPolyShape_meta);
  /* metatable.__index = methods table */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpPolyShape_methods);
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

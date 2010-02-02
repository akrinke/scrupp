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

#include "cpVect-lua.h"

#define check_cpBB(L, index) \
  (cpBB *)luaL_checkudata(L, (index), "cpBB")

static cpBB *push_cpBB (lua_State *L) {
  cpBB *bb = (cpBB *)lua_newuserdata(L, sizeof(cpBB));
  luaL_getmetatable(L, "cpBB");
  lua_setmetatable(L, -2);
  return bb;
}

static int cpBB_new(lua_State *L) {
  cpFloat l = (cpFloat)luaL_checknumber (L, 1);
  cpFloat b = (cpFloat)luaL_checknumber (L, 2);
  cpFloat r = (cpFloat)luaL_checknumber (L, 3);
  cpFloat t = (cpFloat)luaL_checknumber (L, 4);
  cpBB *bb = push_cpBB(L);
  bb->l = l;
  bb->b = b;
  bb->r = r;
  bb->t = t;
  return 1;
}

static int cpBB_intersects(lua_State *L) {
  cpBB *o1 = check_cpBB(L, 1);
  cpBB *o2 = check_cpBB(L, 2);
  lua_pushboolean(L, cpBBintersects(*o1, *o2));
  return 1;
}

static int cpBB_containsBB(lua_State *L) {
  cpBB *o1 = check_cpBB(L, 1);
  cpBB *o2 = check_cpBB(L, 2);
  lua_pushboolean(L, cpBBcontainsBB(*o1, *o2));
  return 1;
}

static int cpBB_containsVect(lua_State *L) {
  cpBB *o1 = check_cpBB(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  lua_pushboolean(L, cpBBcontainsVect(*o1, *o2));
  return 1;
}

static int cpBB_clampVect(lua_State *L) {
  cpBB *o1 = check_cpBB(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect v = cpBBClampVect(*o1, *o2);
  cpVect *o = push_cpVect(L);
  o->x = v.x;
  o->y = v.y;
  return 1;
}

static int cpBB_wrapVect(lua_State *L) {
  cpBB *o1 = check_cpBB(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect v = cpBBWrapVect(*o1, *o2);
  cpVect *o = push_cpVect(L);
  o->x = v.x;
  o->y = v.y;
  return 1;
}

static int cpBB_tostring(lua_State *L) {
  lua_pushfstring(L, "cpBB (%p)", lua_topointer(L, 1));
  return 1;
}

static const luaL_reg cpBB_functions[] = {
  {"newBB", cpBB_new},
  {NULL, NULL}
};

static const luaL_reg cpBB_methods[] = {
  {"intersects",   cpBB_intersects},
  {"containsBB",   cpBB_containsBB},
  {"containsVect", cpBB_containsVect},
  {"clampVect",    cpBB_clampVect},
  {"wrapVect",     cpBB_wrapVect},
  {NULL, NULL}
};

static const luaL_reg cpBB_meta[] = {
  {"__tostring", cpBB_tostring},
  {NULL, NULL}
};

int cpBB_register(lua_State *L) {
  luaL_register(L, NULL, cpBB_functions);
  
  luaL_newmetatable(L, "cpBB");
  luaL_register(L, NULL, cpBB_meta);
  /* metatable.__index = methods */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpBB_methods);
  lua_rawset(L, -3);

  lua_pop(L, 1);
  return 0;
}

  

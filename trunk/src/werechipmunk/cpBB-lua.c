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

static inline cpBB check_cpBB(lua_State *L, int index) {
  cpBB bb = {
    (cpFloat)luaL_checknumber(L, index),
    (cpFloat)luaL_checknumber(L, index+1),
    (cpFloat)luaL_checknumber(L, index+2),
    (cpFloat)luaL_checknumber(L, index+3)
  };
  return bb;
}

static int cpBB_intersects(lua_State *L) {
  cpBB bb1 = check_cpBB(L, 1);
  cpBB bb2 = check_cpBB(L, 5);
  lua_pushboolean(L, cpBBintersects(bb1, bb2));
  return 1;
}

static int cpBB_containsBB(lua_State *L) {
  cpBB bb1 = check_cpBB(L, 1);
  cpBB bb2 = check_cpBB(L, 5);
  lua_pushboolean(L, cpBBcontainsBB(bb1, bb2));
  return 1;
}

static int cpBB_containsVect(lua_State *L) {
  cpBB bb = check_cpBB(L, 1);
  cpVect v = check_cpVect(L, 5);
  lua_pushboolean(L, cpBBcontainsVect(bb, v));
  return 1;
}

static int cpBB_clampVect(lua_State *L) {
  cpBB bb = check_cpBB(L, 1);
  cpVect v = check_cpVect(L, 5);
  push_cpVect(L, cpBBClampVect(bb, v));
  return 2;
}

static int cpBB_wrapVect(lua_State *L) {
  cpBB bb = check_cpBB(L, 1);
  cpVect v = check_cpVect(L, 5);
  push_cpVect(L, cpBBWrapVect(bb, v));
  return 2;
}

static const luaL_reg cpBB_functions[] = {
  {"intersects",   cpBB_intersects},
  {"containsBB",   cpBB_containsBB},
  {"containsVect", cpBB_containsVect},
  {"clampVect",    cpBB_clampVect},
  {"wrapVect",     cpBB_wrapVect},
  {NULL, NULL}
};

int cpBB_register(lua_State *L) {
  lua_pushliteral(L, "bb");
  lua_newtable(L);
  luaL_register(L, NULL, cpBB_functions);
  lua_rawset(L, -3);
  return 0;
}

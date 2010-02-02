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

#include "cpShape-lua.h"

cpShape *check_cpShape (lua_State *L, int index) {
  cpShape *s = (cpShape *)lua_touserdata(L, index);
  /* get table of metatables from the registry */
  lua_pushliteral(L, "cpShapes");
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (s == NULL || !lua_getmetatable(L, index)) {
    luaL_typerror(L, index, "cpShape");
  }
  lua_rawget(L, -2);
  if (lua_isnil(L, -1)) {
    luaL_typerror(L, index, "cpShape");
  }
  /* pop table of metatables and boolean */
  lua_pop(L, 2);
  return s;
}

int cpShape_getBody(lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushliteral(L, "cpBody_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, s->body);
  lua_rawget(L, -2);
  return 1;
}

int cpShape_setRestitution (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpFloat e = (cpFloat)luaL_checknumber(L, 2);
  s->e = e;
  return 0;
}

int cpShape_setFriction (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpFloat u = (cpFloat)luaL_checknumber(L, 2);
  s->u = u;
  return 0;
}

int cpShape_setCollisionType(lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpCollisionType n = (cpCollisionType)luaL_checkint(L, 2);
  s->collision_type = n;
  return 0;
}

int cpShape_gc(lua_State *L) {
  /* no need to check the type */
  cpShape* s = lua_touserdata(L, 1);
  cpShapeFree(s);
  return 0;
}

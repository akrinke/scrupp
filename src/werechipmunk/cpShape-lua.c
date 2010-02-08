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
#include "cpBody-lua.h"
#include "cpVect-lua.h"

cpShape *check_cpShape (lua_State *L, int index) {
  cpShape **ps = (cpShape **)lua_touserdata(L, index);
  /* get table of metatables from the registry */
  lua_pushliteral(L, "cpShapes");
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (ps == NULL || !lua_getmetatable(L, index)) {
    luaL_typerror(L, index, "cpShape");
  }
  lua_rawget(L, -2);
  if (lua_isnil(L, -1)) {
    luaL_typerror(L, index, "cpShape");
  }
  /* pop table of metatables and boolean */
  lua_pop(L, 2);
  return *ps;
}

int cpShape_getBody (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushliteral(L, "cpBody_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, s->body);
  lua_rawget(L, -2);
  return 1;
}

int cpShape_setBody (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpBody *b = check_cpBody(L, 2);
  s->body = b;
  
  /* cpReferences.shape_userdata = body_userdata */
  lua_pushliteral(L, "cpReferences");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, 1);
  lua_pushvalue(L, 2);
  lua_rawset(L, -3);
  lua_pop(L, 1);
  
  return 0;
}

int cpShape_isSensor (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushboolean(L, s->sensor);
  return 1;
}

int cpShape_setSensor (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  luaL_checkany(L, 2);
  s->sensor = lua_toboolean(L, 2);
  return 0;
}

int cpShape_getCollisionType (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushinteger(L, s->collision_type);
  return 1;
}

int cpShape_setCollisionType (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpCollisionType n = (cpCollisionType)luaL_checkint(L, 2);
  s->collision_type = n;
  return 0;
}

int cpShape_getGroup (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushinteger(L, s->group);
  return 1;
}

int cpShape_setGroup (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpGroup g = (cpGroup)luaL_checkint(L, 2);
  s->group = g;
  return 0;  
}

int cpShape_getLayers (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushinteger(L, s->layers);
  return 1;
}

int cpShape_setLayers (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpLayers l = (cpLayers)luaL_checkint(L, 2);
  s->layers = l;
  return 0;
}

int cpShape_getBB (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushnumber(L, (double)s->bb.l);
  lua_pushnumber(L, (double)s->bb.b);
  lua_pushnumber(L, (double)s->bb.r);
  lua_pushnumber(L, (double)s->bb.t);
  return 4;
}

int cpShape_cacheBB (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpShapeCacheBB(s);
  lua_pushnumber(L, (double)s->bb.l);
  lua_pushnumber(L, (double)s->bb.b);
  lua_pushnumber(L, (double)s->bb.r);
  lua_pushnumber(L, (double)s->bb.t);
  return 4;
}

int cpShape_getElasticity (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushnumber(L, (double)s->e);
  return 1;
}

int cpShape_setElasticity (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpFloat e = (cpFloat)luaL_checknumber(L, 2);
  s->e = e;
  return 0;
}

int cpShape_getFriction (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  lua_pushnumber(L, (double)s->u);
  return 1;
}

int cpShape_setFriction (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpFloat u = (cpFloat)luaL_checknumber(L, 2);
  s->u = u;
  return 0;
}

int cpShape_getSurfaceVelocity (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpVect v = s->surface_v;
  push_cpVect(L, v);
  return 2;
}

int cpShape_setSurfaceVelocity (lua_State *L) {
  cpShape *s = check_cpShape(L, 1);
  cpVect v = check_cpVect(L, 2);
  s->surface_v = v;
  return 2;
}  

int cpShape_resetIdCounter (lua_State *L) {
  cpResetShapeIdCounter();
  return 0;
}

int cpShape_gc (lua_State *L) {
  /* no need to check the type */
  cpShape **ps = lua_touserdata(L, 1);
  cpShapeFree(*ps);
  return 0;
}

static const luaL_reg cpShape_functions[] = {
  {"resetShapeIdCounter", cpShape_resetIdCounter},
  {NULL, NULL}
};

int cpShape_register (lua_State *L) {
  luaL_register(L, NULL, cpShape_functions);
  return 0;
}

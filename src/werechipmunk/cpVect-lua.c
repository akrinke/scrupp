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

#include <chipmunk.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpVect-lua.h"

cpVect *push_cpVect(lua_State *L) {
  cpVect *v = (cpVect *)lua_newuserdata(L, sizeof(cpVect));
  luaL_getmetatable(L, "cpVect");
  lua_setmetatable(L, -2);
  return v;
}

static int cpVect_new(lua_State *L) {
  cpFloat x = 0.0f;
  cpFloat y = 0.0f;
  /* number of arguments */
  int n = lua_gettop(L);
 
  if (n == 1) {
    if (lua_isnumber(L, 1)) {
      cpFloat a = (cpFloat)luaL_checknumber(L, 1);
      cpVect vi = cpvforangle(a);
      x = vi.x;
      y = vi.y;
    } else {
      cpVect *vi = check_cpVect(L, 1);
      x = vi->x;
      y = vi->y;
    }
  }

  if (n == 2) {
    x = (cpFloat)luaL_checknumber (L, 1);
    y = (cpFloat)luaL_checknumber (L, 2);
  }
  cpVect *v = push_cpVect(L);
  v->x = x;
  v->y = y;
  
  return 1;
}

static int cpVect_setx (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  v->x = luaL_checknumber(L, 2);
  lua_settop(L, 1);
  return 0;
}

static int cpVect_sety (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  v->y = (cpFloat)luaL_checknumber(L, 2);
  lua_settop(L, 1);
  return 0;
}

static int cpVect_getx (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  lua_pushnumber(L, (double)v->x);
  return 1;
}

static int cpVect_gety (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  lua_pushnumber(L, (double)v->y);
  return 1;
}

static int cpVect_length (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  lua_pushnumber(L, (double)cpvlength(*v));
  return 1;
}

static int cpVect_lengthsq (lua_State *L) {
  cpVect *v = check_cpVect(L, 1);
  lua_pushnumber(L, (double)cpvlengthsq(*v));
  return 1;
}

static int cpVect_add (lua_State *L){
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect *sum  = push_cpVect(L);
  sum->x = o1->x + o2->x;
  sum->y = o1->y + o2->y;
  return 1;
}

static int cpVect_sub (lua_State *L){
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect *sum  = push_cpVect(L);
  sum->x = o1->x - o2->x;
  sum->y = o1->y - o2->y;
  return 1;
}

static int cpVect_neg (lua_State *L){
  cpVect *o = check_cpVect(L, 1);
  o->x = -o->x;
  o->y = -o->y;
  return 0;
}


static int cpVect_mult(lua_State *L){
  cpVect *o = check_cpVect(L, 1);
  cpFloat m = (cpFloat)luaL_checknumber(L, 2);
  o->x = o->x*m;
  o->y = o->y*m;
  return 0;
}

static int cpVect_perp(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect v = cpvperp(*o1);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}


static int cpVect_lerp(lua_State *L) {
  cpVect *o1 = (cpVect *)check_cpVect(L, 1);
  cpVect *o2 = (cpVect *)check_cpVect(L, 2);
  cpFloat t = (cpFloat)luaL_checknumber(L, 3);
  cpVect v = cpvlerp(*o1, *o2, t);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}

static int cpVect_project(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect v=cpvproject(*o1, *o2);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}


static int cpVect_normalise(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect v = cpvnormalize(*o1);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}

static int cpVect_normalise_safe(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect v = cpvnormalize_safe(*o1);
  o1->x = v.x;
  o1->y = v.y;  
  return 0;
}

static int cpVect_clamp(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpFloat len = (cpFloat)luaL_checknumber(L, 2);
  cpVect v = cpvclamp(*o1, len);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}


static int cpVect_rotate(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect v = cpvrotate(*o1, *o2);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}

static int cpVect_unrotate(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpVect v = cpvunrotate(*o1, *o2);
  o1->x = v.x;
  o1->y = v.y;
  return 0;
}

static int cpVect_dot(lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  lua_pushnumber(L, (double)cpvdot(*o1, *o2));
  return 1;
}

static int cpVect_cross(lua_State *L){
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  lua_pushnumber(L, (double)cpvcross(*o1, *o2));
  return 1;
}

static int cpVect_tostring (lua_State *L) {
  lua_pushfstring(L, "cpVect (%p)", lua_topointer(L, 1));
  return 1;
}

static int cpVect_dist (lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  lua_pushnumber(L, (double)cpvdist(*o1, *o2));
  return 1;
}

static int cpVect_distsq (lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  lua_pushnumber(L, (double)cpvdistsq(*o1, *o2));
  return 1;
}
static int cpVect_toangle (lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  lua_pushnumber(L, (double)cpvtoangle(*o1));
  return 1;
}

static int cpVect_near (lua_State *L) {
  cpVect *o1 = check_cpVect(L, 1);
  cpVect *o2 = check_cpVect(L, 2);
  cpFloat d = (cpFloat)luaL_checknumber(L, 3);
  lua_pushboolean(L, (double)cpvnear(*o1,*o2,d) );
  return 1;
}

static const luaL_reg cpVect_methods[] = {
  {"new",            cpVect_new},
  {"setx",           cpVect_setx},
  {"sety",           cpVect_sety},
  {"getx",           cpVect_getx},
  {"gety",           cpVect_gety},
  {"add",            cpVect_add},
  {"sub",            cpVect_sub},
  {"neg",            cpVect_neg},
  {"mult",           cpVect_mult},
  {"dot",            cpVect_dot},
  {"cross",          cpVect_dot},
  {"perp",           cpVect_perp},
  {"project",        cpVect_project},
  {"rotate",         cpVect_rotate},
  {"unrotate",       cpVect_unrotate},
  {"length",         cpVect_length},
  {"lengthsq",       cpVect_lengthsq},
  {"lerp",           cpVect_lerp},
  {"clamp",          cpVect_clamp},
  {"normalize",      cpVect_normalise},
  {"normalize_safe", cpVect_normalise_safe},
  {"dist",           cpVect_dist},
  {"distsq",         cpVect_distsq},
  {"near",           cpVect_near},
  {"toangle",        cpVect_toangle},
  {NULL, NULL}
};

static const luaL_reg cpVect_meta[] = {
  {"__tostring", cpVect_tostring},
  {"__add",      cpVect_add},
  {"__sub",      cpVect_sub},
  {NULL, NULL}
};


int cpVect_register (lua_State *L) {
  lua_pushliteral(L, "cpVect");
  lua_newtable(L);  
  luaL_register(L, NULL, cpVect_methods);
  lua_rawset(L, -3);

  luaL_newmetatable(L, "cpVect");          /* create metatable for cpVect, and add it to the Lua registry */
  luaL_register(L, NULL, cpVect_meta);     /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 1);                      /* drop metatable */
  return 0;                           /* return methods on the stack */
}

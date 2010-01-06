/*    Copyright (c) 2009 Mr C.Camacho
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

static cpBB *push_cpBB (lua_State *L) {
  cpBB *bb = (cpBB *)lua_newuserdata(L, sizeof(cpBB));
  luaL_getmetatable(L, "cpBB");
  lua_setmetatable(L, -2);
  return bb;
}

static int cpBB_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 4 ) return luaL_error(L, "Got %d arguments expected d", n);
   
   double l = luaL_checknumber (L, 1);      
   double b = luaL_checknumber (L, 2);
   double r = luaL_checknumber (L, 3);      
   double t = luaL_checknumber (L, 4);
   cpBB *bb = push_cpBB(L);
   bb->l = l;
   bb->b = b;
   bb->r = r;
   bb->t = t;
   
   return 1;
}

static cpBB *check_cpBB (lua_State *L, int index) {
  cpBB *b;
  luaL_checktype(L, index, LUA_TUSERDATA);
  b = (cpBB *)luaL_checkudata(L, index, "cpBB");
  if (b == NULL) luaL_typerror(L, index, "cpBB");
  return b;
}

static int cpBB_intersects(lua_State *L) {
   cpBB *o1 = check_cpBB(L,1);
   cpBB *o2 = check_cpBB(L,2);
   lua_pushboolean(L,cpBBintersects(*o1,*o2));
   return 1;
}

static int cpBB_containsBB(lua_State *L) {
   cpBB *o1 = check_cpBB(L,1);
   cpBB *o2 = check_cpBB(L,2);
   lua_pushboolean(L,cpBBcontainsBB(*o1,*o2));
   return 1;
}

static int cpBB_containsVect(lua_State *L) {
   cpBB *o1 = check_cpBB(L,1);
   cpVect *o2 = check_cpVect(L,2);
   lua_pushboolean(L,cpBBcontainsVect(*o1,*o2));
   return 1;
}

static int cpBB_clampVect(lua_State *L) {
   cpBB *o1 = check_cpBB(L,1);
   cpVect *o2 = check_cpVect(L,2);
   cpVect v=cpBBClampVect(*o1,*o2);
	cpVect *o=push_cpVect(L);
   o->x=v.x;o->y=v.y;
   return 1;
}

static int cpBB_wrapVect(lua_State *L) {
   cpBB *o1 = check_cpBB(L,1);
   cpVect *o2 = check_cpVect(L,2);
   cpVect v=cpBBWrapVect(*o1,*o2);
	cpVect *o=push_cpVect(L);
   o->x=v.x;o->y=v.y;
   return 1;
}

static const luaL_reg cpBB_methods[] = {
  {"new",            cpBB_new},
  {"intersects",     cpBB_intersects},
  {"containsBB",     cpBB_containsBB},
  {"containsVect",   cpBB_containsVect},
  {"clampVect",      cpBB_clampVect},
  {"wrapVect",       cpBB_wrapVect},
  {0, 0}
};  
  
int cpBB_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}
static const luaL_reg cpBB_meta[] = {  
   {"__tostring", cpBB_tostring}, 
//   {"__gc",       cpBB_gc}, 
   {0, 0}
};

int cpBB_register (lua_State *L) {
  luaL_openlib(L, "cpBB", cpBB_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpBB");          /* create metatable for cpBB, and add it to the Lua registry */
  luaL_openlib(L, 0, cpBB_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 2);                      /* drop metatable */
  return 0;                           /* return methods on the stack */
}

  

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

#include "cpMisc-lua.h"

cpShape *check_cpShape (lua_State *L, int index) {
  cpShape *b;
  luaL_checktype(L, index, LUA_TUSERDATA);
  b = (cpShape *)deref(luaL_checkudata(L, index, "cpShape"));
  if (b == NULL) luaL_typerror(L, index, "cpShape");
  return b;
}


int cpShape_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   lua_pushstring(L,"cpShape");
   return 1;
}


int cpShape_setRestitution (lua_State *L) {
   cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,1)); 
   double n = luaL_checknumber(L,2);
   shp->e = n;
   return 0;
}

int cpShape_setFriction (lua_State *L) {
   cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,1));
   double n = luaL_checknumber(L,2);
   shp->u = n;
   return 0;
}

int cpShape_setCollisionType(lua_State *L) {
   cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,1));
   unsigned int n = luaL_checknumber(L,2);
   shp->collision_type=n;
   return 0;
}

int cpShape_getBody(lua_State *L) {
   cpShape* shp =(cpShape*)deref((void*)lua_topointer(L,1));
   lua_getglobal(L,"__cpBody_ptrs");
   lua_pushlightuserdata(L,shp->body);
   lua_gettable(L,-2);
   return 1;
}

int cpShape_free(lua_State *L) {
   cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,1));
   lua_getglobal(L,"__cpShape_ptrs");
   lua_pushlightuserdata(L,shp);
   lua_pushnil(L);
   lua_rawset(L,-3);
   cpShapeFree(shp);
}   


static const luaL_reg cpShape_methods[] = {
//  {"new",               cpShape_new}, // should only use PolyShape etc
  {"setRestitution",    cpShape_setRestitution},
  {"setFriction",       cpShape_setFriction},
  {"setCollisionType",  cpShape_setCollisionType},
  {"getBody",           cpShape_getBody},
  {"free",              cpShape_free},
  {0, 0}
};   
static const luaL_reg cpShape_meta[] = {  
   {"__tostring",       cpShape_tostring}, 
   {0, 0}                       
};

int cpShape_register (lua_State *L) {
  luaL_openlib(L, "cpShape", cpShape_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpShape");          /* create metatable for cpShape, and add it to the Lua registry */
  luaL_openlib(L, 0, cpShape_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 2);                      /* drop metatable */
  
  lua_newtable(L); 
  lua_setglobal(L, "__cpShape_ptrs");

  return 0;                           /* return methods on the stack */
}



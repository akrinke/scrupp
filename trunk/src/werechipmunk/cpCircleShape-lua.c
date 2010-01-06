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
#include "cpBody-lua.h"
#include "cpShape-lua.h"

static cpCircleShape *push_cpCircleShape (lua_State *L) {
   cpCircleShape *bb=cpCircleShapeAlloc(); // initialise later
   cpCircleShape **pbb = (cpCircleShape **)lua_newuserdata(L, sizeof(cpCircleShape*));
   *pbb = bb;

   luaL_getmetatable(L, "cpCircleShape");
   lua_setmetatable(L, -2);

   lua_getglobal(L,"__cpShape_ptrs");
   lua_pushlightuserdata(L, bb);   // table index
   lua_pushvalue(L,-3); // previously created table of *cpBody pointers to userdata
   lua_rawset(L, -3); // update the table
   lua_pop(L,1);

  return bb;
}

cpCircleShape* check_cpCircleShape (lua_State *L, int index) {
  cpCircleShape *bb;
  luaL_checktype(L, index, LUA_TUSERDATA);
  cpCircleShape **pbb =(cpCircleShape**)luaL_checkudata(L, index, "cpCircleShape");
  bb=*pbb;
  if (bb == NULL) luaL_typerror(L, index, "cpCircleShape");
  return (cpCircleShape*)bb;
}

static int cpCircleShape_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 3 ) return luaL_error(L, "Got %d arguments expected 3", n);
   
   cpBody* body = check_cpBody (L, 1);      
   double radius = luaL_checknumber (L, 2);
   cpVect* offset = check_cpVect (L, 3);      

   cpCircleShape *circle = push_cpCircleShape(L); // have to allocate onto stack
//cpCircleShapeInit(cpCircleShape *circle, cpBody *body, cpFloat radius, cpVect offset)  
   cpCircleShapeInit(circle, body, radius, *offset);        // so initialise it manually
 
   return 1;
}

 
static int cpCircleShape_getOffset(lua_State *L) {
   cpCircleShape* cs = check_cpCircleShape (L, 1);
   cpVect *v = push_cpVect(L);
   cpVect vo  = cpCircleShapeGetOffset((cpShape*)cs);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}
 
static int cpCircleShape_getRadius(lua_State *L) {
   cpCircleShape* cs = check_cpCircleShape (L, 1);
   lua_pushnumber(L, cpCircleShapeGetRadius((cpShape*)cs));
   return 1;
}
 
static const luaL_reg cpCircleShape_methods[] = {
  {"new",               cpCircleShape_new},
  {"getOffset",         cpCircleShape_getOffset},
  {"getRadius",         cpCircleShape_getRadius},
  {"setRestitution",    cpShape_setRestitution},
  {"setFriction",       cpShape_setFriction},
  {"setCollisionType",  cpShape_setCollisionType},
  {"getBody",           cpShape_getBody},
  {"free",              cpShape_free},
  {0, 0}
};   

int cpCircleShape_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}
static const luaL_reg cpCircleShape_meta[] = {  
   {"__tostring", cpCircleShape_tostring}, 
   {0, 0}                       
};

int cpCircleShape_register (lua_State *L) {
  luaL_openlib(L, "cpCircleShape", cpCircleShape_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpCircleShape");          /* create metatable for cpCircleShape, and add it to the Lua registry */
  luaL_openlib(L, 0, cpCircleShape_meta, 0);    /* fill metatable */
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



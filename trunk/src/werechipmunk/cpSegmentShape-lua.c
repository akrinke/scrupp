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
#include "cpMisc-lua.h"


static cpSegmentShape *push_cpSegmentShape (lua_State *L) {
   cpSegmentShape *ss=cpSegmentShapeAlloc();
   cpSegmentShape **pss=(cpSegmentShape**)lua_newuserdata(L, sizeof(cpSegmentShape *));
   *pss=ss;
   luaL_getmetatable(L, "cpSegmentShape");
   lua_setmetatable(L, -2);  
   
   lua_pushliteral(L, "werechip.cpShape_ptrs");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushlightuserdata(L,ss);
   lua_pushvalue(L,-3);
   lua_rawset(L,-3);
   lua_pop(L,1);
   
   return ss;
}

static cpSegmentShape *check_cpSegmentShape (lua_State *L, int index) {
   cpSegmentShape *ss;
   luaL_checktype(L,index,LUA_TUSERDATA);
   ss=(cpSegmentShape*)deref((void*)luaL_checkudata(L,index,"cpSegmentShape"));
   if (ss == NULL) luaL_typerror(L,index,"cpSegmentShape");
   return ss;
}

static int cpSegmentShape_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 4 ) return luaL_error(L, "Got %d arguments expected 4", n);
   
   cpBody* body = check_cpBody (L, 1);
   cpVect* a = check_cpVect (L, 2); 
   cpVect* b = check_cpVect (L, 3);      
   double radius = luaL_checknumber (L, 4);

   cpSegmentShape *seg = push_cpSegmentShape(L); // have to allocate onto stack
//cpSegmentShape* cpSegmentShapeInit(cpSegmentShape *seg, cpBody *body, cpVect a, cpVect b, cpFloat radius) 
   cpSegmentShapeInit(seg, body, *a, *b, radius);        // so initialise it manually
   lua_pushliteral(L, "werechip.references");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushvalue(L, -2);
   lua_pushvalue(L, 1);
   lua_rawset(L, -3);
   lua_pop(L, 1);
   return 1;
}

 
static int cpSegmentShape_getA(lua_State *L) {
   cpSegmentShape* cs = check_cpSegmentShape (L, 1);
   cpVect *v = push_cpVect(L);
   cpVect vo  = cpSegmentShapeGetA((cpShape*)cs);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}
 
static int cpSegmentShape_getB(lua_State *L) {
   cpSegmentShape* cs = check_cpSegmentShape (L, 1);
   cpVect *v = push_cpVect(L);
   cpVect vo  = cpSegmentShapeGetB((cpShape*)cs);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}
 
static int cpSegmentShape_getNormal(lua_State *L) {
   cpSegmentShape* cs = check_cpSegmentShape (L, 1);
   cpVect *v = push_cpVect(L);
   cpVect vo  = cpSegmentShapeGetNormal((cpShape*)cs);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}
 
static int cpSegmentShape_getRadius(lua_State *L) {
   cpSegmentShape* cs = check_cpSegmentShape (L, 1);
   lua_pushnumber(L, cpSegmentShapeGetRadius((cpShape*)cs));
   return 1;
}
 
static const luaL_reg cpSegmentShape_methods[] = {
  {"new",               cpSegmentShape_new},
  {"getA",              cpSegmentShape_getA},
  {"getB",              cpSegmentShape_getB},
  {"getNormal",         cpSegmentShape_getNormal},
  {"getRadius",         cpSegmentShape_getRadius},
  {"setRestitution",    cpShape_setRestitution},
  {"setFriction",       cpShape_setFriction},
  {"setCollisionType",  cpShape_setCollisionType},
  {"getBody",           cpShape_getBody},
  {0, 0}
};   

int cpSegmentShape_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   char buff[32];
   sprintf(buff,"%p->%p",lua_touserdata(L,1),deref(lua_touserdata(L,1)));
   lua_pushfstring(L,"cpSegmentShape (%s)",buff);
   return 1;
}
static const luaL_reg cpSegmentShape_meta[] = {  
   {"__tostring", cpSegmentShape_tostring}, 
   {"__gc",       cpShape_gc},
   {0, 0}                       
};

int cpSegmentShape_register (lua_State *L) {
  luaL_openlib(L, "cpSegmentShape", cpSegmentShape_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpSegmentShape");          /* create metatable for cpSegmentShape, and add it to the Lua registry */
  luaL_openlib(L, 0, cpSegmentShape_meta, 0);    /* fill metatable */
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



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


static cpPolyShape *push_cpPolyShape (lua_State *L) {
   cpPolyShape *ps=cpPolyShapeAlloc(); //initialise later
   cpPolyShape **psp=(cpPolyShape **)lua_newuserdata(L, sizeof(cpPolyShape *)); // a pointer to the pointer....
   *psp=ps;
   luaL_getmetatable(L, "cpPolyShape");
   lua_setmetatable(L, -2);
   
   lua_pushliteral(L, "werechip.cpShape_ptrs");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushlightuserdata(L,ps);  // the C pointer (light user data) is user as the index
   lua_pushvalue(L,-3); // the UserData indexed by the light user data
   lua_rawset(L, -3); // update the table
   lua_pop(L,1); // restore stack after table op
   
   return ps;
}
static cpPolyShape *check_cpPolyShape (lua_State *L, int index) {
  cpPolyShape *ps;
  luaL_checktype(L, index, LUA_TUSERDATA);
  ps=(cpPolyShape *)deref((void*)luaL_checkudata(L, index, "cpPolyShape"));
  if (ps == NULL) luaL_typerror(L, index, "cpPolyShape");
  return ps;  
}

static int cpPolyShape_new(lua_State *L) {
   int n = lua_gettop(L);  // Number of arguments
   if (n != 4 ) return luaL_error(L, "Got %d arguments expected 4", n);
   
   cpBody* body = check_cpBody (L, 1);      
   int nVerts = luaL_checknumber (L, 2);

   cpVect* verts = (cpVect *)malloc(nVerts * sizeof(cpVect));

   int i;
   for (i=0;i<nVerts;i++) {
      
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      cpVect* c=check_cpVect(L,-1);
      lua_pop(L,1);   

      verts[i].x=c->x;
      verts[i].y=c->y;
   }
   
   cpVect* offset = check_cpVect (L, 4);      

   cpPolyShape *poly = push_cpPolyShape(L); // have to allocate onto stack
//cpPolyShape *cpPolyShapeInit(cpPolyShape *poly, cpBody *body, int numVerts, cpVect *verts, cpVect offset)
   cpPolyShapeInit(poly, body, nVerts,verts,*offset);        // so initialise it manually
   free(verts);
   lua_pushliteral(L, "werechip.references");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushvalue(L, -2);
   lua_pushvalue(L, 1);
   lua_rawset(L, -3);
   lua_pop(L, 1);

   return 1;
}

 //cpVect cpPolyShapeGetVert(cpShape *shape, int index)
static int cpPolyShape_getVert(lua_State *L) {
   cpPolyShape* cs = check_cpPolyShape (L, 1);
   int i = luaL_checknumber(L,2);
   cpVect *v = push_cpVect(L);
   cpVect vo  = cpPolyShapeGetVert((cpShape*)cs,i);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}

static int cpPolyShape_getNumVerts(lua_State *L) {
   cpPolyShape* cs = check_cpPolyShape (L, 1);
   lua_pushnumber(L, cpPolyShapeGetNumVerts((cpShape*)cs));
   return 1;
}
 
static const luaL_reg cpPolyShape_methods[] = {
  {"new",               cpPolyShape_new},
  {"getVert",           cpPolyShape_getVert},
  {"getNumVerts",       cpPolyShape_getNumVerts},
  {"setRestitution",    cpShape_setRestitution},
  {"setFriction",       cpShape_setFriction},
  {"setCollisionType",  cpShape_setCollisionType},
  {"getBody",           cpShape_getBody},
  {0, 0}
};   

int cpPolyShape_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   char buff[32];
   sprintf(buff,"%p->%p",lua_touserdata(L,1),deref(lua_touserdata(L,1)));
   lua_pushfstring(L,"cpPolyShape (%s)",buff);
   return 1;
}

// TODO TODO TODO add GC code vects and axes need freeing
static const luaL_reg cpPolyShape_meta[] = {  
   {"__tostring",          cpPolyShape_tostring}, 
   {"__gc",       cpShape_gc},
   {0, 0}                       
};


   
int cpPolyShape_register (lua_State *L) {
  luaL_openlib(L, "cpPolyShape", cpPolyShape_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpPolyShape");          /* create metatable for cpPolyShape, and add it to the Lua registry */
  luaL_openlib(L, 0, cpPolyShape_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */

   // no pointer to userdata table as we use werechip.cpShape_ptrs 
                                  
  lua_pop(L, 2);                      /* drop metatable */
  return 0;                           /* return methods on the stack */
}



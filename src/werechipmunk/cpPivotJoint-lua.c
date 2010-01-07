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
#include "cpConstraint-lua.h"


static cpPivotJoint *push_cpPivotJoint (lua_State *L) {
   cpPivotJoint *bb=cpPivotJointAlloc(); // initialise later
   cpPivotJoint **pbb = (cpPivotJoint **)lua_newuserdata(L, sizeof(cpPivotJoint*));
   *pbb = bb;

   luaL_getmetatable(L, "cpPivotJoint");
   lua_setmetatable(L, -2);

  return bb;
}

static int cpPivotJoint_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 4 ) return luaL_error(L, "Got %d arguments expected 4 rtfm error ", n);
   
   cpBody* b1 = check_cpBody (L, 1);
   cpBody* b2 = check_cpBody (L, 2);
   cpVect* p1 = check_cpVect (L, 3);
   cpVect* p2 = check_cpVect (L, 4);

   cpConstraint *s = (cpConstraint*)push_cpPivotJoint(L); // 
//cpPivotJoint *cpPivotJointInit(cpPivotJoint *joint, cpBody *a, cpBody *b, cpVect pivot)
   cpPivotJointInit((cpPivotJoint*)s, b1,b2,*p1,*p2);        // so initialise it manually
   
   cpConstraint_store_refs(L);
   return 1;
}
   
static const luaL_reg cpPivotJoint_methods[] = {
	{"new",               cpPivotJoint_new},
	{0, 0}
};   

int cpPivotJoint_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}

static const luaL_reg cpPivotJoint_meta[] = {  
   {"__tostring", cpPivotJoint_tostring}, 
   {"__gc", cpConstraint_gc},
   {0, 0}                       
};

int cpPivotJoint_register (lua_State *L) {
  luaL_openlib(L, "cpPivotJoint", cpPivotJoint_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpPivotJoint");          /* create metatable for cpPivotJoint, and add it to the Lua registry */
  luaL_openlib(L, 0, cpPivotJoint_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
   													  
// no joint pointer table uses constraints
  
  lua_pop(L, 2);                      /* drop metatable */
  return 0;                           /* return methods on the stack */
}


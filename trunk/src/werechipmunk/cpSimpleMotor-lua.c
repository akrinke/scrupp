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


static cpSimpleMotor *push_cpSimpleMotor (lua_State *L) {
   cpSimpleMotor *bb=cpSimpleMotorAlloc(); // initialise later
   cpSimpleMotor **pbb = (cpSimpleMotor **)lua_newuserdata(L, sizeof(cpSimpleMotor*));
   *pbb = bb;

   luaL_getmetatable(L, "cpSimpleMotor");
   lua_setmetatable(L, -2);

   lua_getglobal(L,"__cpConstraint_ptrs");
   lua_pushlightuserdata(L, bb);   // table index
   lua_pushvalue(L,-3); // previously created table of *cpSimpleMotor pointers to userdata
   lua_rawset(L, -3); // update the table
   lua_pop(L,1);

  return bb;
}

static int cpSimpleMotor_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 3 ) return luaL_error(L, "Got %d arguments expected 3, 2 bodies and rate ", n);
   
   cpBody* b1 = check_cpBody (L, 1);
   cpBody* b2 = check_cpBody (L, 2);
   float rate = luaL_checknumber (L, 3);

   cpConstraint *s = (cpConstraint*)push_cpSimpleMotor(L); // 
   cpSimpleMotorInit((cpSimpleMotor*)s, b1,b2,rate);        // so initialise it manually
   
   return 1;
}

   
static const luaL_reg cpSimpleMotor_methods[] = {
	{"new",               cpSimpleMotor_new},
	{"free",              cpConstraint_free},
	{0, 0}
};   

int cpSimpleMotor_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}
static const luaL_reg cpSimpleMotor_meta[] = {  
   {"__tostring", cpSimpleMotor_tostring}, 
   {0, 0}                       
};

int cpSimpleMotor_register (lua_State *L) {
  luaL_openlib(L, "cpSimpleMotor", cpSimpleMotor_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpSimpleMotor");          /* create metatable for cpSimpleMotor, and add it to the Lua registry */
  luaL_openlib(L, 0, cpSimpleMotor_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
   													  
// no joint pointer table uses constraints pointer table
  
  lua_pop(L, 2);                      /* drop metatable */
  return 0;                           /* return methods on the stack */
}



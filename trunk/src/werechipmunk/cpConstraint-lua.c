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
#include "cpMisc-lua.h"

cpConstraint* check_cpConstraint (lua_State *L, int index) {
  cpConstraint *bb;
  luaL_checktype(L, index, LUA_TUSERDATA);
  cpConstraint **pbb =(cpConstraint**)luaL_checkudata(L, index, "cpConstraint");
  bb=*pbb;
  if (bb == NULL) luaL_typerror(L, index, "cpConstraint");
  return (cpConstraint*)bb;
}

void cpConstraint_store_refs(lua_State *L) {
   /* store references to the two bodies */
   lua_pushliteral(L, "werechip.references");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushvalue(L, -2);
   lua_newtable(L);
   lua_pushvalue(L, 1);   /* push body 1 on the stack */
   lua_rawseti(L, -2, 1); /* t[1] = body 1 */
   lua_pushvalue(L, 2);   /* push body 2 on the stack */
   lua_rawseti(L, -2, 2); /* t[2] = body 2 */
   lua_rawset(L,-3);
   lua_pop(L, 1);
}  

inline cpConstraint* get_cpConstraint (lua_State *L, int index) {
   cpConstraint *c =(cpConstraint*)deref((void*)lua_topointer(L, index));
   return c;
}
   
static const luaL_reg cpConstraint_methods[] = {
//  {"new",               cpConstraint_new},  //should only be created by joints 
//	{"free",               cpConstraint_free}, // called by child instances
	{0, 0}
};   

static int cpConstraint_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}

int cpConstraint_gc(lua_State *L){
   cpConstraint *b = check_cpConstraint(L, 1);
   cpConstraintFree(b);
   return 0;
}

static const luaL_reg cpConstraint_meta[] = {  
   {"__tostring", cpConstraint_tostring}, 
   {"__gc",       cpConstraint_gc},
   {0, 0}                       
};

int cpConstraint_register (lua_State *L) {
  luaL_openlib(L, "cpConstraint", cpConstraint_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpConstraint");          /* create metatable for cpConstraint, and add it to the Lua registry */
  luaL_openlib(L, 0, cpConstraint_meta, 0);    /* fill metatable */
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



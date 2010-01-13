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

inline void* deref(void **ptrptr) {
   return *ptrptr;
}

// only here for debugging lua calls
void stackdump(lua_State* l)
{
    int i;
    int top = lua_gettop(l);

    printf("total in stack %d\n",top);

    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(l, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
               printf("string: '%s'\n", lua_tostring(l, i));
               break;
            case LUA_TBOOLEAN:  /* booleans */
               printf("boolean %s\n",lua_toboolean(l, i) ? "true" : "false");
               break;
            case LUA_TNUMBER:  /* numbers */
               printf("number: %g\n", lua_tonumber(l, i));
               break;
            case LUA_TUSERDATA:
               printf("userdata: %p\n",lua_topointer(l,i));
               break;
            case LUA_TLIGHTUSERDATA:
               printf("luserdata: %p\n",lua_topointer(l,i));
               break;
            default:  /* other values */
               printf("%s\n", lua_typename(l, t));
               break;
        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

static int cpMisc_initChipmunk(lua_State *L) {
   cpInitChipmunk();   
   return 0;
}

//cpFloat cpMomentForCircle(cpFloat m, cpFloat r1, cpFloat r2, cpVect offset)
static int cpMisc_calcCircleMoment(lua_State *L) {
   double m = luaL_checknumber(L,1);
   double r1 = luaL_checknumber(L,2);
   double r2 = luaL_checknumber(L,3);
   cpVect* os=(cpVect *)luaL_checkudata(L, 4, "cpVect");
   lua_pushnumber(L, cpMomentForCircle(m, r1, r1, *os));
   return 1;
}

//cpFloat cpMomentForSegment(cpFloat m, cpVect a, cpVect b)
static int cpMisc_calcSegmentMoment(lua_State *L) {
   double m = luaL_checknumber(L,1);
   cpVect* a=(cpVect *)luaL_checkudata(L, 2, "cpVect");
   cpVect* b=(cpVect *)luaL_checkudata(L, 3, "cpVect");
   lua_pushnumber(L, cpMomentForSegment(m, *a, *b));
   return 1;
}

// cpFloat cpMomentForPoly(cpFloat m, const int numVerts, cpVect *verts, cpVect offset)
static int cpMisc_calcPolyMoment(lua_State *L) {
   double m = luaL_checknumber(L,1);
   int nVerts= luaL_checkinteger(L,2);
   /* TODO TODO check that arg 3 is a table */
   cpVect* verts = (cpVect *)calloc(nVerts, sizeof(cpVect));
   
   int i;
   for (i=0;i<nVerts;i++) {
      
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      cpVect* c=(cpVect *)luaL_checkudata(L, -1, "cpVect");
      lua_pop(L,1);   

      verts[i].x=c->x;
      verts[i].y=c->y;
   }
   
   cpVect* os=(cpVect *)luaL_checkudata(L, 4, "cpVect");
   
   lua_pushnumber(L, cpMomentForPoly(m,nVerts,verts,*os));
   free(verts);
   return 1;
}

static const luaL_reg cpMisc_methods[] = {
//  {"new",               cpMisc_new},
  {"initChipmunk",      cpMisc_initChipmunk},
  {"calcPolyMoment",    cpMisc_calcPolyMoment},
  {"calcSegmentMoment", cpMisc_calcSegmentMoment},
  {"calcCircleMoment",  cpMisc_calcCircleMoment},
  {0, 0}
};   

int cpMisc_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   return 0;
}
static const luaL_reg cpMisc_meta[] = {  
   {"__tostring", cpMisc_tostring}, 
   {0, 0}                       
};

int cpMisc_register (lua_State *L) {
  luaL_openlib(L, "cpMisc", cpMisc_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpMisc");          /* create metatable for cpMisc, and add it to the Lua registry */
  luaL_openlib(L, 0, cpMisc_meta, 0);    /* fill metatable */
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

// entry point for lua lib loading
int luaopen_werechip(lua_State *lstate) {
   cpVect_register(lstate);
   cpBB_register(lstate);
   cpArbiter_register(lstate);
   cpBody_register(lstate);
   cpShape_register(lstate);
   cpCircleShape_register(lstate);
   cpSegmentShape_register(lstate);     
   cpPolyShape_register(lstate);
   cpSpace_register(lstate);
   cpMisc_register(lstate);
   cpConstraint_register(lstate);
   cpPinJoint_register(lstate);
   cpPivotJoint_register(lstate);
   cpSimpleMotor_register(lstate);
   
   /* create a table with weak keys
    * this table stores all references to userdatas 
    * (shapes, bodies, constraints) a userdata has
    */
   lua_pushliteral(lstate, "werechip.references");
   lua_newtable(lstate);
   lua_pushvalue(lstate, -1);               /* duplicate the table */
   lua_pushliteral(lstate, "__mode");
   lua_pushliteral(lstate, "k");
   lua_rawset(lstate, -3);                  /* table.__mode = 'k' */
   lua_setmetatable(lstate, -2);            /* table.metatable = table */
   lua_settable(lstate, LUA_REGISTRYINDEX);
   return 0;
}


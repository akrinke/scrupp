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

#include "chipmunk.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpVect-lua.h"

static int cpMisc_calcCircleMoment (lua_State *L) {
  cpFloat m = (cpFloat)luaL_checknumber(L, 1);
  cpFloat r1 = (cpFloat)luaL_checknumber(L, 2);
  cpFloat r2 = (cpFloat)luaL_checknumber(L, 3);
  cpVect os = check_cpVect(L, 4);
  lua_pushnumber(L, (double)cpMomentForCircle(m, r1, r2, os));
  return 1;
}

static int cpMisc_calcSegmentMoment (lua_State *L) {
  cpFloat m = (cpFloat)luaL_checknumber(L, 1);
  cpVect a = check_cpVect(L, 2);
  cpVect b = check_cpVect(L, 4);
  lua_pushnumber(L, (double)cpMomentForSegment(m, a, b));
  return 1;
}

static int cpMisc_calcPolyMoment (lua_State *L) {
  cpFloat m = (cpFloat)luaL_checknumber(L, 1);
  luaL_checktype(L, 2, LUA_TTABLE);
  int n = lua_objlen(L, 2);
  luaL_argcheck(L, (n % 2 == 0) && (n > 4), 2, "at least 3 pairs of coordinates are requires");
  cpVect offset = check_cpVect(L, 3);

  cpVect *verts = (cpVect *)calloc(n/2, sizeof(cpVect));
  cpVect *tv = verts;
  
  int i;
  for (i=1; i<n; i=i+2) {
    lua_pushinteger(L, i);
    lua_gettable(L, 2);
    lua_pushinteger(L, i+1);
    lua_gettable(L, 2);
    tv->x = (cpFloat)luaL_checknumber(L, -2);
    tv->y = (cpFloat)luaL_checknumber(L, -1);
    tv++;
    lua_pop(L, 2);
  }

  lua_pushnumber(L, (double)cpMomentForPoly(m, n/2, verts, offset));
  free(verts);
  return 1;
}

static const luaL_reg cpMisc_functions[] = {
  {"calcPolyMoment",    cpMisc_calcPolyMoment},
  {"calcSegmentMoment", cpMisc_calcSegmentMoment},
  {"calcCircleMoment",  cpMisc_calcCircleMoment},
  {NULL, NULL}
};

static int cpMisc_register (lua_State *L) {
  luaL_register(L, NULL, cpMisc_functions);
  return 0; 
}

// entry point for lua lib loading
int luaopen_werechip (lua_State *L) {
  cpInitChipmunk();

  /* create table with the metatables of the various constraints */
  lua_pushliteral(L, "cpConstraints");
  lua_newtable(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  /* create table with the metatables of the various shapes */
  lua_pushliteral(L, "cpShapes");
  lua_newtable(L);
  lua_rawset(L, LUA_REGISTRYINDEX);
   
  /* create a table with weak values which maps 
     shape pointers (light userdata) to full shape userdatas */
  lua_pushliteral(L, "cpShape_ptrs");
  lua_newtable(L);
  /* duplicate the table */
  lua_pushvalue(L, -1);
  /* table.__mode = 'v' */
  lua_pushliteral(L, "__mode");
  lua_pushliteral(L, "v");
  lua_rawset(L, -3);
  /* table.metatable = table */
  lua_setmetatable(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  /* create a table with weak values which maps 
     body pointers (light userdata) to full body userdata */
  lua_pushliteral(L, "cpBody_ptrs");
  lua_newtable(L);
  /* duplicate the table */
  lua_pushvalue(L, -1);
  /* table.__mode = 'v' */
  lua_pushliteral(L, "__mode");
  lua_pushliteral(L, "v");
  lua_rawset(L, -3);
  /* table.metatable = table */
  lua_setmetatable(L, -2);
  lua_settable(L, LUA_REGISTRYINDEX);

  /* create a table with weak keys
     this table stores all references to userdatas 
     (shapes, bodies, constraints) a userdata has */
  lua_pushliteral(L, "cpReferences");
  lua_newtable(L);
  /* duplicate the table */
  lua_pushvalue(L, -1);
  /* table.__mode = 'k' */
  lua_pushliteral(L, "__mode");
  lua_pushliteral(L, "k");
  lua_rawset(L, -3);
  /* table.metatable = table */
  lua_setmetatable(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  cpArbiter_register(L);
  cpBB_register(L);
  cpBody_register(L);
  cpMisc_register(L);
  cpSpace_register(L);
  cpVect_register(L);
  
  /* shapes */
  cpShape_register(L);
  cpCircleShape_register(L);
  cpSegmentShape_register(L);
  cpPolyShape_register(L);
  
  /* constraints */
  cpDampedRotarySpring_register(L);
  cpDampedSpring_register(L);
  cpGearJoint_register(L);
  cpGrooveJoint_register(L);
  cpPinJoint_register(L);
  cpPivotJoint_register(L);
  cpRatchetJoint_register(L);
  cpRotaryLimitJoint_register(L);
  cpSimpleMotor_register(L);
  cpSlideJoint_register(L);
  
  return 1;
}

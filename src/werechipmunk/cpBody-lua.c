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

#include "cpBody-lua.h"
#include "cpMisc-lua.h"
#include "cpVect-lua.h"

static cpBody *push_cpBody (lua_State *L) {
  cpBody *b = cpBodyAlloc();
  cpBody **pb = (cpBody **)lua_newuserdata(L, sizeof(cpBody*));
  *pb = b;

  luaL_getmetatable(L, "cpBody");
  lua_setmetatable(L, -2);

  lua_pushliteral(L, "cpBody_ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  /* cpBody_ptrs.body_ptr = userdata */
  lua_pushlightuserdata(L, b);
  lua_pushvalue(L, -3);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  return b;
}

static int cpBody_newStatic (lua_State *L) {
  cpBody *b = push_cpBody(L);
  cpBodyInit(b, INFINITY, INFINITY);
  return 1;
}

static int cpBody_new (lua_State *L) {
  cpFloat m = (cpFloat)luaL_checknumber (L, 1);      
  cpFloat i = (cpFloat)luaL_checknumber (L, 2);
  cpBody *b = push_cpBody(L);
  cpBodyInit(b, m, i);
  return 1;
}

WCP_DefineBodyGetterSetterFloat(Mass, Mass);
WCP_DefineBodyGetterSetterFloat(Moment, Moment);
WCP_DefineBodyGetterSetterVect(Position, Pos);
WCP_DefineBodyGetterSetterVect(Velocity, Vel);
WCP_DefineBodyGetterSetterVect(Force, Force);
WCP_DefineBodyGetterSetterFloat(Angle, Angle);
WCP_DefineBodyGetterSetterFloat(AngularVelocity, AngVel);
WCP_DefineBodyGetterSetterFloat(Torque, Torque);

static int cpBody_getRotation (lua_State *L){
  cpBody *b = check_cpBody(L, 1);
  push_cpVect(L, cpBodyGetRot(b));
  return 2;
}

static int cpBody_getLocal2World (lua_State *L){
  cpBody *b = check_cpBody(L, 1);
  cpVect vi = check_cpVect(L, 2);
  push_cpVect(L, cpBodyLocal2World(b, vi));
  return 2;
}

static int cpBody_getWorld2Local (lua_State *L){
  cpBody *b = check_cpBody(L, 1);
  cpVect vi = check_cpVect(L, 2);
  push_cpVect(L, cpBodyWorld2Local(b, vi));
  return 2;
}

static int cpBody_applyImpulse (lua_State *L){
  cpBody *b = check_cpBody(L, 1);
  cpVect j = check_cpVect(L, 2);
  cpVect r = check_cpVect(L, 4);
  cpBodyApplyImpulse(b, j, r);
  return 0;
}

static int cpBody_resetForces (lua_State *L){ 
  cpBody *b = check_cpBody(L, 1);
  cpBodyResetForces(b);
  return 0;
}

static int cpBody_applyForce (lua_State *L){
  cpBody *b = check_cpBody(L, 1);
  cpVect f = check_cpVect(L, 2);
  cpVect r = check_cpVect(L, 4);
  cpBodyApplyForce(b, f, r);
  return 0;
}

static int cpBody_applyDampedSpring (lua_State *L){
  cpBody *a = check_cpBody(L, 1);
  cpBody *b = check_cpBody(L, 2);
  cpVect an1 = check_cpVect(L, 3);
  cpVect an2 = check_cpVect(L, 5);
  cpFloat rlen = (cpFloat)luaL_checknumber(L, 7);
  cpFloat k = (cpFloat)luaL_checknumber(L, 8);
  cpFloat dmp = (cpFloat)luaL_checknumber(L, 9);
  cpFloat dt = (cpFloat)luaL_checknumber(L, 10);
  cpApplyDampedSpring(a, b, an1, an2, rlen, k, dmp, dt);
  return 0;
}

static int cpBody_gc (lua_State *L) {
  /* no need to check the type */
  cpBody **b = lua_touserdata(L, 1);
  cpBodyFree(*b);
  return 0;
}

static int cpBody_tostring (lua_State *L) {
  lua_pushfstring(L, "cpBody (%p)", lua_topointer(L, 1));
  return 1;
  }

static const luaL_reg cpBody_functions[] = {
  {"newBody",       cpBody_new},
  {"newStaticBody", cpBody_newStatic},
  {NULL, NULL}
};

static const luaL_reg cpBody_methods[] = {
  WCP_METHOD(Body, getMass),
  WCP_METHOD(Body, setMass),
  WCP_METHOD(Body, getMoment),
  WCP_METHOD(Body, setMoment),
  WCP_METHOD(Body, getPosition),
  WCP_METHOD(Body, setPosition),
  WCP_METHOD(Body, getVelocity),
  WCP_METHOD(Body, setVelocity),
  WCP_METHOD(Body, getForce),
  WCP_METHOD(Body, setForce),
  WCP_METHOD(Body, getAngle),
  WCP_METHOD(Body, setAngle),
  WCP_METHOD(Body, getAngularVelocity),
  WCP_METHOD(Body, setAngularVelocity),
  WCP_METHOD(Body, getTorque),
  WCP_METHOD(Body, setTorque),
  WCP_METHOD(Body, getRotation),
  WCP_METHOD(Body, getLocal2World),
  WCP_METHOD(Body, getWorld2Local),
  WCP_METHOD(Body, applyImpulse),
  WCP_METHOD(Body, resetForces),
  WCP_METHOD(Body, applyForce),
  WCP_METHOD(Body, applyDampedSpring),
  {NULL, NULL}
};

static const luaL_reg cpBody_meta[] = {
  {"__gc",       cpBody_gc},
  {"__tostring", cpBody_tostring},
  {NULL, NULL}
};

int cpBody_register (lua_State *L) {
  luaL_register(L, NULL, cpBody_functions);
  
  luaL_newmetatable(L, "cpBody");
  luaL_register(L, NULL, cpBody_meta);
  /* metatable.__index = methods*/
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpBody_methods);
  lua_rawset(L, -3);
  lua_pop(L, 1);
  
  return 0;
}

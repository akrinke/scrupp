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

#ifndef CPCONSTRAINT_LUA_H
#define CPCONSTRAINT_LUA_H

#define WCP_DefineConstraintGetterSetterFloat(TYPE, NAME, CPNAME) \
static int cp##TYPE##_get##NAME (lua_State *L) { \
  cp##TYPE *constraint = check_cp##TYPE(L, 1); \
  lua_pushnumber(L, constraint->CPNAME); \
  return 1; \
} \
static int cp##TYPE##_set##NAME (lua_State *L) { \
  cp##TYPE *constraint = check_cp##TYPE(L, 1); \
  cpFloat value = (cpFloat)luaL_checknumber(L, 2); \
  constraint->CPNAME = value; \
  return 0; \
}

#define WCP_DefineConstraintGetterSetterVect(TYPE, NAME, CPNAME) \
static int cp##TYPE##_get##NAME (lua_State *L) { \
  cp##TYPE *constraint = check_cp##TYPE(L, 1); \
  push_cpVect(L, constraint->CPNAME); \
  return 2; \
} \
static int cp##TYPE##_set##NAME (lua_State *L) { \
  cp##TYPE *constraint = check_cp##TYPE(L, 1); \
  cpVect v = check_cpVect(L, 2); \
  constraint->CPNAME = v; \
  return 0; \
}

#define DEFINE_CONSTRAINT_METHODS \
  {"getBodyA", cpConstraint_getBodyA}, \
  {"setBodyA", cpConstraint_setBodyA}, \
  {"getBodyB", cpConstraint_getBodyB}, \
  {"setBodyB", cpConstraint_setBodyB}, \
  {"getMaxForce", cpConstraint_getMaxForce}, \
  {"setMaxForce", cpConstraint_setMaxForce}, \
  {"getBiasCoefficient", cpConstraint_getBiasCoefficient}, \
  {"setBiasCoefficient", cpConstraint_setBiasCoefficient}, \
  {"getMaxBias", cpConstraint_getMaxBias}, \
  {"setMaxBias", cpConstraint_setMaxBias}

cpConstraint *check_cpConstraint(lua_State *L, int index);
int cpConstraint_getBodyA (lua_State *L);
int cpConstraint_setBodyA (lua_State *L);
int cpConstraint_getBodyB (lua_State *L);
int cpConstraint_setBodyB (lua_State *L);
int cpConstraint_getMaxForce (lua_State *L);
int cpConstraint_setMaxForce (lua_State *L);
int cpConstraint_getBiasCoefficient (lua_State *L);
int cpConstraint_setBiasCoefficient (lua_State *L);
int cpConstraint_getMaxBias (lua_State *L);
int cpConstraint_setMaxBias (lua_State *L);
int cpConstraint_gc (lua_State *L);

#endif

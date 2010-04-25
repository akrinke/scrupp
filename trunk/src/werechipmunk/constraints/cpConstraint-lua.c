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

cpConstraint *check_cpConstraint (lua_State *L, int index) {
  cpConstraint **pc = (cpConstraint **)lua_touserdata(L, index);
  /* get table of metatables from the registry */
  lua_pushliteral(L, "cpConstraints");
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (pc == NULL || !lua_getmetatable(L, index)) {
    luaL_typerror(L, index, "cpConstraint");
  }
  lua_rawget(L, -2);
  if (lua_isnil(L, -1)) {
    luaL_typerror(L, index, "cpConstraint");
  }
  /* pop table of metatables and boolean */
  lua_pop(L, 2);
  return *pc;
}

int cpConstraint_getBodyA (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  lua_pushliteral(L, "cpBody__ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, c->a);
  lua_rawget(L, -2);
  return 1;
}

int cpConstraint_setBodyA (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  cpBody *b = check_cpBody(L, 2);
  c->a = b;
  return 0;
}

int cpConstraint_getBodyB (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  lua_pushliteral(L, "cpBody__ptrs");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, c->b);
  lua_rawget(L, -2);
  return 1;
}

int cpConstraint_setBodyB (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  cpBody *b = check_cpBody(L, 2);
  c->b = b;
  return 0;
}

int cpConstraint_getMaxForce (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  lua_pushnumber(L, (double)c->maxForce);
  return 1;
}

int cpConstraint_setMaxForce (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  cpFloat maxForce = (cpFloat)luaL_checknumber(L, 2);
  c->maxForce = maxForce;
  return 0;
}

int cpConstraint_getBiasCoefficient (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  lua_pushnumber(L, (double)c->biasCoef);
  return 1;
}

int cpConstraint_setBiasCoefficient (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  cpFloat biasCoef = (cpFloat)luaL_checknumber(L, 2);
  c->biasCoef = biasCoef;
  return 0;
}

int cpConstraint_getMaxBias (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  lua_pushnumber(L, (double)c->maxBias);
  return 1;
}

int cpConstraint_setMaxBias (lua_State *L) {
  cpConstraint *c = check_cpConstraint(L, 1);
  cpFloat maxBias = (cpFloat)luaL_checknumber(L, 2);
  c->maxBias = maxBias;
  return 0;
}

int cpConstraint_gc (lua_State *L) {
  /* no need to check the type */
  cpConstraint **c = (cpConstraint **)lua_touserdata(L, 1);
  cpConstraintFree(*c);
  return 0;
}

void cpConstraint_store_refs (lua_State *L) {
  /* store references to the two bodies */
  lua_pushliteral(L, "cpReferences");
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushvalue(L, -2);
  lua_newtable(L);
  lua_pushvalue(L, 1);   /* push body 1 on the stack */
  lua_rawseti(L, -2, 1); /* t[1] = body 1 */
  lua_pushvalue(L, 2);   /* push body 2 on the stack */
  lua_rawseti(L, -2, 2); /* t[2] = body 2 */
  lua_rawset(L,-3);
  lua_pop(L, 1);
}

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

#ifndef CPBODY_LUA_H
#define CPBODY_LUA_H

#define check_cpBody(L, index) \
  *(cpBody **)luaL_checkudata(L, (index), "cpBody")
  
#define WCP_DefineBodyGetterSetterFloat(NAME, CPNAME) \
static int cpBody_get##NAME (lua_State *L) { \
  cpBody *b = check_cpBody(L, 1); \
  lua_pushnumber(L, (double)cpBodyGet##CPNAME(b)); \
  return 1; \
} \
static int cpBody_set##NAME (lua_State *L) { \
  cpBody *b = check_cpBody(L, 1); \
  cpFloat value = (cpFloat)luaL_checknumber(L, 2); \
  cpBodySet##CPNAME(b, value); \
  return 0; \
}

#define WCP_DefineBodyGetterSetterVect(NAME, CPNAME) \
static int cpBody_get##NAME (lua_State *L) { \
  cpBody *b = check_cpBody(L, 1); \
  push_cpVect(L, cpBodyGet##CPNAME(b)); \
  return 2; \
} \
static int cpBody_set##NAME (lua_State *L) { \
  cpBody *b = check_cpBody(L, 1); \
  cpVect v = check_cpVect(L, 2); \
  cpBodySet##CPNAME(b, v); \
  return 0; \
}

#endif

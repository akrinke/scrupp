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

#include <chipmunk.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpVect-lua.h"

#define WCP_Define1VectTo1Num(name)       static int cpVect_##name(lua_State *L){lua_pushnumber(L, (double)cpv##name(check_cpVect(L, 1))); return 1;}
#define WCP_Define1VectTo1Vect(name)      static int cpVect_##name(lua_State *L){push_cpVect(L, cpv##name(check_cpVect(L, 1))); return 2;}
#define WCP_Define1Vect1NumTo1Vect(name)  static int cpVect_##name(lua_State *L){push_cpVect(L, cpv##name(check_cpVect(L, 1), (cpFloat)luaL_checknumber(L, 3))); return 2;}
#define WCP_Define2VectsTo1Num(name)      static int cpVect_##name(lua_State *L){lua_pushnumber(L, (double)cpv##name(check_cpVect(L, 1), check_cpVect(L, 3))); return 1;}
#define WCP_Define2VectsTo1Vect(name)     static int cpVect_##name(lua_State *L){push_cpVect(L, cpv##name(check_cpVect(L, 1), check_cpVect(L, 3))); return 2;}
#define WCP_Define2Vects1NumTo1Vect(name) static int cpVect_##name(lua_State *L){push_cpVect(L, cpv##name(check_cpVect(L, 1), check_cpVect(L, 3), (cpFloat)luaL_checknumber(L, 5))); return 2;}

inline void push_cpVect(lua_State *L, cpVect v) {
  lua_pushnumber(L, (double)v.x);
  lua_pushnumber(L, (double)v.y);
}

inline cpVect check_cpVect(lua_State *L, int index) {
  cpVect v = {(cpFloat)luaL_checknumber(L, index), (cpFloat)luaL_checknumber(L, index+1)};
  return v;
}

WCP_Define2VectsTo1Num(dot);
WCP_Define2VectsTo1Num(cross);
WCP_Define1VectTo1Vect(perp);
WCP_Define1VectTo1Vect(rperp);
WCP_Define2VectsTo1Vect(project);
WCP_Define2VectsTo1Vect(rotate);
WCP_Define2VectsTo1Vect(unrotate);
WCP_Define1VectTo1Num(length);
WCP_Define1VectTo1Num(lengthsq);
WCP_Define2Vects1NumTo1Vect(lerp);
WCP_Define2Vects1NumTo1Vect(lerpconst);
WCP_Define2Vects1NumTo1Vect(slerp);
WCP_Define2Vects1NumTo1Vect(slerpconst);
WCP_Define1Vect1NumTo1Vect(clamp);
WCP_Define1VectTo1Vect(normalize);
WCP_Define1VectTo1Vect(normalize_safe);
WCP_Define2VectsTo1Num(dist);
WCP_Define2VectsTo1Num(distsq);

static int cpVect_near(lua_State *L) {
  cpVect v1 = check_cpVect(L, 1);
  cpVect v2 = check_cpVect(L, 3);
  cpFloat d = (cpFloat)luaL_checknumber(L, 5);
  lua_pushboolean(L, cpvnear(v1, v2, d));
  return 1;
}

static int cpVect_forangle(lua_State *L) {
  cpFloat a = (cpFloat)luaL_checknumber(L, 1);
  push_cpVect(L, cpvforangle(a));
  return 2;
}

WCP_Define1VectTo1Num(toangle);

static const luaL_reg cpVect_functions[] = {
  {"dot",            cpVect_dot},
  {"cross",          cpVect_dot},
  {"perp",           cpVect_perp},
  {"rperp",          cpVect_rperp},
  {"project",        cpVect_project},
  {"rotate",         cpVect_rotate},
  {"unrotate",       cpVect_unrotate},
  {"length",         cpVect_length},
  {"lengthsq",       cpVect_lengthsq},
  {"lerp",           cpVect_lerp},
  {"lerpconst",      cpVect_lerpconst},
  {"slerp",          cpVect_slerp},
  {"slerpconst",     cpVect_slerpconst},
  {"clamp",          cpVect_clamp},
  {"normalize",      cpVect_normalize},
  {"normalize_safe", cpVect_normalize_safe},
  {"dist",           cpVect_dist},
  {"distsq",         cpVect_distsq},
  {"near",           cpVect_near},
  {"forangle",       cpVect_forangle},
  {"toangle",        cpVect_toangle},
  {NULL, NULL}
};

int cpVect_register (lua_State *L) {
  lua_pushliteral(L, "vect");
  lua_newtable(L);  
  luaL_register(L, NULL, cpVect_functions);
  lua_rawset(L, -3);
  return 0;
}

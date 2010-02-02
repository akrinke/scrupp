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
#include <stdio.h>

#include "chipmunk.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpBody-lua.h"
#include "cpConstraint-lua.h"
#include "cpShape-lua.h"
#include "cpVect-lua.h"

#define check_cpSpace(L, index) \
  *(cpSpace **)luaL_checkudata(L, (index), "cpSpace")

static inline void push_cpSpace (lua_State *L, cpSpace *space) {
  cpSpace **ptr = (cpSpace **)lua_newuserdata(L, sizeof(cpSpace *));
  *ptr = space;
  luaL_getmetatable(L, "cpSpace");
  lua_setmetatable(L, -2);
}

static inline void create_reference(lua_State *L) {
  /* get the environment table of the space userdata */
  lua_getfenv(L, 1);
  /* push the userdata which should be referenced */
  lua_pushvalue(L, 2);
  lua_pushboolean(L, 1);
  lua_rawset(L, -3);
  lua_pop(L, 1);
}

static inline void remove_reference(lua_State *L) {
  /* get the environment table of the space userdata */
  lua_getfenv(L, 1);
  /* push the userdata whose reference should be removed */
  lua_pushvalue(L, 2);
  lua_pushnil(L);
  lua_rawset(L, -3);
  lua_pop(L, 1);
}

static int cpSpace_new(lua_State *L) {
  int i = luaL_checkint(L, 1);
  cpSpace *space = cpSpaceAlloc();
  cpSpaceInit(space);
  push_cpSpace(L, space);
  /* set the environment of the space to a new table */
  lua_newtable(L);
  lua_setfenv(L, -2);
  return 1;
}

static int cpSpace_removeConstraint(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpConstraint *c = check_cpConstraint(L, 2);
  cpSpaceRemoveConstraint(space,c);
  remove_reference(L);
  return 0;
}


static int cpSpace_removeBody(lua_State *L) {
  cpSpace* space = check_cpSpace(L, 1);
  cpBody* body = check_cpBody(L, 2);
  cpSpaceRemoveBody(space, body);
  remove_reference(L);
  return 0;
}

static int cpSpace_removeStaticShape(lua_State *L) {
  cpSpace* space = check_cpSpace(L, 1);
  cpShape* shape = check_cpShape(L, 2);
  cpSpaceRemoveStaticShape(space,shape);
  remove_reference(L);
  return 0;
}

static int cpSpace_removeShape(lua_State *L) {
  cpSpace* space = check_cpSpace(L, 1);
  cpShape* shape = check_cpShape(L, 2);
  cpSpaceRemoveShape(space, shape);
  remove_reference(L);
  return 0;
}

static int cpSpace_addShape(lua_State *L) {
  cpSpace* space = check_cpSpace(L, 1);
  cpShape* shape = check_cpShape(L, 2);
  cpSpaceAddShape(space, shape);
  create_reference(L);
  return 0;
}

static int cpSpace_addBody(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpBody *body = check_cpBody(L, 2);
  cpSpaceAddBody(space, body);
  create_reference(L);
  return 0;
}

static int cpSpace_addStaticShape(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpShape *shape = check_cpShape(L, 2);
  cpSpaceAddStaticShape(space, shape);
  create_reference(L);
  return 0;
}

static int cpSpace_addConstraint(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpConstraint *c = check_cpConstraint(L, 2);
  cpSpaceAddConstraint(space, c);
  create_reference(L);
  return 0;
}

static int cpSpace_step(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpFloat dt = (cpFloat)luaL_checknumber(L, 2);
  cpSpaceStep(space, dt);
  return 0;
}

static int cpSpace_setGravity(lua_State *L) {
  cpSpace *space = check_cpSpace(L,1);
  cpVect *v = check_cpVect(L,2);
  space->gravity = *v;
  return 0;
}

static int cpSpace_freeChildren(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpSpaceFreeChildren(space);
  return 0;
}

static void CollisionHandler(lua_State *L, cpArbiter *arb, cpSpace *space, int func_ref) {
  push_cpArbiter(L, arb);
  lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
  /* push copy of arbiter */
  lua_pushvalue(L, -2);
  push_cpSpace(L, space);

  if (lua_pcall(L, 2, 1, 0) != 0) {
      luaL_error(L, "Error running werechipmunk callback:\n\t%s\n", lua_tostring(L, -1));
  }
  /* access to the arbiter is no longer possible */
  cpArbiter **ptr = lua_touserdata(L, -2);
  *ptr = NULL;
  /* return value of callback stays on top of the stack */
}

typedef struct {
  lua_State *L;
  int BeginFunc;
  int PreSolveFunc;
  int PostSolveFunc;
  int SeparateFunc;
} callbackData;

static int CollisionBeginFunc(cpArbiter *arb, cpSpace *space, void *data) {
  callbackData* cbd = (callbackData*)data;
  CollisionHandler(cbd->L, arb, space, cbd->BeginFunc);
  return lua_toboolean(cbd->L, -1);
}

static int CollisionPreSolveFunc(cpArbiter *arb, cpSpace *space, void *data) {
  callbackData* cbd = (callbackData*)data;
  CollisionHandler(cbd->L, arb, space, cbd->PreSolveFunc);
  return lua_toboolean(cbd->L, -1);
}

static void CollisionPostSolveFunc(cpArbiter *arb, cpSpace *space, void *data) {
    callbackData* cbd = (callbackData*)data;
    CollisionHandler(cbd->L, arb, space, cbd->PostSolveFunc);
}

static void CollisionSeparateFunc(cpArbiter *arb, cpSpace *space, void *data) {
  callbackData* cbd = (callbackData*)data;
  CollisionHandler(cbd->L, arb, space, cbd->SeparateFunc);
}

static int cpSpace_addCollisionHandler(lua_State *L) {
  lua_settop(L, 7);
  cpSpace *space = check_cpSpace(L, 1);
  cpCollisionType a = (cpCollisionType)luaL_checkint(L, 2);
  cpCollisionType b = (cpCollisionType)luaL_checkint(L, 3);
  callbackData *cbdata = malloc(sizeof(callbackData));
  cbdata->L = L;
  cbdata->SeparateFunc = luaL_ref(L, LUA_REGISTRYINDEX);
  cbdata->PostSolveFunc = luaL_ref(L, LUA_REGISTRYINDEX);
  cbdata->PreSolveFunc = luaL_ref(L, LUA_REGISTRYINDEX);
  cbdata->BeginFunc = luaL_ref(L, LUA_REGISTRYINDEX);
  cpSpaceAddCollisionHandler(
    space, a,b, 
    cbdata->BeginFunc     == LUA_REFNIL ? NULL : CollisionBeginFunc,
    cbdata->PreSolveFunc  == LUA_REFNIL ? NULL : CollisionPreSolveFunc,
    cbdata->PostSolveFunc == LUA_REFNIL ? NULL : CollisionPostSolveFunc,
    cbdata->SeparateFunc  == LUA_REFNIL ? NULL : CollisionSeparateFunc,
    cbdata
  );
  return 0;
}

static int cpSpace_removeCollisionHandler(lua_State *L) {
  cpSpace *space = check_cpSpace(L, 1);
  cpCollisionType a = (cpCollisionType)luaL_checkint(L, 2);
  cpCollisionType b = (cpCollisionType)luaL_checkint(L, 3);
  struct{cpCollisionType a, b;} ids = {a, b};
  cpCollisionHandler *old_handler = cpHashSetRemove(space->collFuncSet, CP_HASH_PAIR(a, b), &ids);
  free(old_handler->data);
  cpfree(old_handler);
}

static int cpSpace_gc(lua_State *L) {
  cpSpace *space = check_cpSpace(L,1);
  cpSpaceFree(space);
}

static int cpSpace_tostring (lua_State *L) {
  lua_pushfstring(L, "cpSpace (%p)", lua_topointer(L, 1));
  return 0;
}

static const struct luaL_reg cpSpace_functions[] = {
  {"newSpace", cpSpace_new},
  {NULL, NULL}
};

static const luaL_reg cpSpace_methods[] = {
  {"addShape",               cpSpace_addShape},
  {"addStaticShape",         cpSpace_addStaticShape},
  {"addBody",                cpSpace_addBody},
  {"addConstraint",          cpSpace_addConstraint},
  {"step",                   cpSpace_step},
  {"setGravity",             cpSpace_setGravity},
  {"addCollisionHandler",    cpSpace_addCollisionHandler},
  {"removeCollisionHandler", cpSpace_removeCollisionHandler},
  {"removeStaticShape",      cpSpace_removeStaticShape},
  {"removeShape",            cpSpace_removeShape},
  {"removeBody",             cpSpace_removeBody},
  {"removeConstraint",       cpSpace_removeConstraint},
  {"freeChildren",           cpSpace_freeChildren},
  {NULL, NULL}
};

static const luaL_reg cpSpace_meta[] = {
  {"__gc",       cpSpace_gc},
  {"__tostring", cpSpace_tostring},
};

int cpSpace_register (lua_State *L) {
  luaL_register(L, NULL, cpSpace_functions);
  
  luaL_newmetatable(L, "cpSpace");
  luaL_register(L, NULL, cpSpace_meta);
  /* metatable.__index = methods */
  lua_pushliteral(L, "__index");
  lua_newtable(L);
  luaL_register(L, NULL, cpSpace_methods);
  lua_rawset(L, -3);
  lua_pop(L, 1);
  
  return 0;
}

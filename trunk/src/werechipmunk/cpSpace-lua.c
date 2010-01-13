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
#include <stdio.h>

#include "chipmunk.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cpArbiter-lua.h"
#include "cpVect-lua.h"
#include "cpBody-lua.h"
#include "cpMisc-lua.h"
#include "cpConstraint-lua.h"

void push_cpSpace (lua_State *L, cpSpace *space) {
	cpSpace **ptr = (cpSpace **)lua_newuserdata(L, sizeof(cpSpace *));
	*ptr = space;
	luaL_getmetatable(L, "cpSpace");
	lua_setmetatable(L, -2);
}

cpSpace *check_cpSpace (lua_State *L, int index) {
   cpSpace **pp;
	luaL_checktype(L, index, LUA_TUSERDATA);
   pp = (cpSpace **)luaL_checkudata(L, index, "cpSpace");
   if (*pp == NULL) luaL_typerror(L, index, "cpSpace");
   return *pp;
}

static inline void create_reference(lua_State *L) {
   lua_getfenv(L, 1);    /* get the environment table of the space userdata */
   lua_pushvalue(L, 2);  /* push the userdata which should be referenced */
   lua_pushboolean(L, 1);
   lua_rawset(L, -3);
   lua_pop(L, 1);
}

static inline void remove_reference(lua_State *L) {
   lua_getfenv(L, 1);    /* get the environment table of the space userdata */
   lua_pushvalue(L, 2); /* push the userdata whose reference should be removed */
   lua_pushnil(L);
   lua_rawset(L, -3);
   lua_pop(L, 1);
}

static int cpSpace_new(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1 ) return luaL_error(L, "Got %d arguments expected 1, iterations ", n);
	int i = luaL_checkinteger (L, 1);
	cpSpace *space = cpSpaceAlloc();
	cpSpaceInit(space);
	push_cpSpace(L, space);
   /* set the environment of the space to a new table */
   lua_newtable(L);
   lua_setfenv(L, -2);
	return 1;
}

int cpSpace_tostring (lua_State *L) {
	// TODO eventually provide text of xml tag representing
	// this object and its properties
	return 0;
}
/* TODO wrap constraints
WIP ! not all joints done
*/

static int cpSpace_removeConstraint(lua_State *L) {
//void cpSpaceRemoveConstraint(cpSpace *space, cpConstraint *constraint)
	cpSpace* space = check_cpSpace(L,1);
	cpConstraint* c = get_cpConstraint(L,2); 
	cpSpaceRemoveConstraint(space,c);
   remove_reference(L);
	return 0;
}


static int cpSpace_removeBody(lua_State *L) {
//void cpSpaceRemoveBody(cpSpace *space, cpShape *shape)
	cpSpace* space = check_cpSpace(L,1);
	cpBody* bdy = check_cpBody(L,2); 
	cpSpaceRemoveBody(space,bdy);
   remove_reference(L);
	return 0;
}

static int cpSpace_removeStaticShape(lua_State *L) {
//void cpSpaceRemoveStaticShape(cpSpace *space, cpShape *shape)
	cpSpace* space = check_cpSpace(L,1);
	cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,2)); 
	cpSpaceRemoveStaticShape(space,shp);
   remove_reference(L);
	return 0;
}

static int cpSpace_removeShape(lua_State *L) {
//void cpSpaceRemoveShape(cpSpace *space, cpShape *shape)
	cpSpace* space = check_cpSpace(L,1);
	cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,2)); 
	cpSpaceRemoveShape(space,shp);
   remove_reference(L);
	return 0;
}

static int cpSpace_addShape(lua_State *L) {
	cpSpace* space = check_cpSpace(L,1);
	cpShape* shp = (cpShape*)deref((void*)lua_topointer(L,2)); 
	cpSpaceAddShape(space,shp);
   create_reference(L);
	return 0;
}

static int cpSpace_addBody(lua_State *L) {
	cpSpace *space = check_cpSpace(L,1);
	cpBody *body = (cpBody *)check_cpBody(L,2);
	cpSpaceAddBody(space,body);
   create_reference(L);
	return 0;
}

static int cpSpace_addStaticShape(lua_State *L) {
	cpSpace *space = check_cpSpace(L,1);
	cpShape *shp = (cpShape*)deref((void*)lua_topointer(L,2)); // can't check for cpshape as could be cpPolyShape or others
	cpSpaceAddStaticShape(space,shp);
   create_reference(L);
	return 0;
}

//void cpSpaceAddConstraint(cpSpace *space, cpConstraint *constraint)
static int cpSpace_addConstraint(lua_State *L) {
	cpSpace *space = check_cpSpace(L,1);
	cpConstraint *c = get_cpConstraint(L,2);
	cpSpaceAddConstraint(space,c);
   create_reference(L);
	return 0;
}

static int cpSpace_step(lua_State *L) {
	 cpSpace *space = check_cpSpace(L,1);
	 double dt = luaL_checknumber(L,2);
	 cpSpaceStep(space, dt);
	 return 0;
}

static int cpSpace_setGravity(lua_State *L) {
	cpSpace *space = check_cpSpace(L,1);
	cpVect *v = check_cpVect(L,2);
	space->gravity=*v;
	return 0;
}

//void cpSpaceFreeChildren(cpSpace *space)
static int cpSpace_freeChildren(lua_State *L) {
    cpSpace *space = check_cpSpace(L,1);
    cpSpaceFreeChildren(space);
    lua_pushliteral(L, "werechip.references");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1); /* push the space userdata on the stack */
    lua_newtable(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    return 0;
}

static int cpSpace_gc(lua_State *L) {
   cpSpace *space = check_cpSpace(L,1);
   cpSpaceFree(space);
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
	ptr = NULL;
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

//void cpSpaceAddCollisionPairFunc(cpSpace *space, cpCollisionType a, cpCollisionType b, cpCollFunc func, void *data) 
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
		cbdata->BeginFunc				== LUA_REFNIL ? NULL : CollisionBeginFunc,
		cbdata->PreSolveFunc			== LUA_REFNIL ? NULL : CollisionPreSolveFunc,
		cbdata->PostSolveFunc		== LUA_REFNIL ? NULL : CollisionPostSolveFunc,
		cbdata->SeparateFunc			== LUA_REFNIL ? NULL : CollisionSeparateFunc,
		cbdata
	);
	return 0;
}

static int cpSpace_removeCollisionHandler(lua_State *L) {
	cpSpace *space = check_cpSpace(L, 1);
	cpCollisionType a = (cpCollisionType)luaL_checkint(L,2);
	cpCollisionType b = (cpCollisionType)luaL_checkint(L,3);
	struct{cpCollisionType a, b;} ids = {a, b};
	cpCollisionHandler *old_handler = cpHashSetRemove(space->collFuncSet, CP_HASH_PAIR(a, b), &ids);
	free(old_handler->data);
	cpfree(old_handler);
}

static const luaL_reg cpSpace_methods[] = {
	{"new",							cpSpace_new},
	{"addShape",					cpSpace_addShape},
	{"addStaticShape",			cpSpace_addStaticShape},
	{"addBody",						cpSpace_addBody},
	{"addConstraint",				cpSpace_addConstraint},
	{"step",							cpSpace_step},
	{"setGravity",					cpSpace_setGravity},
	{"addCollisionHandler",		cpSpace_addCollisionHandler},
	{"removeCollisionHandler", cpSpace_removeCollisionHandler},
	{"removeStaticShape",		cpSpace_removeStaticShape},
	{"removeShape",				cpSpace_removeShape},
	{"removeBody",					cpSpace_removeBody},
	{"removeConstraint",			cpSpace_removeConstraint},
	{"freeChildren",				cpSpace_freeChildren},
	{0, 0}
};	

static const luaL_reg cpSpace_meta[] = {  
	{"__tostring", cpSpace_tostring}, 
	{"__gc", cpSpace_gc},
	{0, 0}
};

int cpSpace_register (lua_State *L) {
  luaL_openlib(L, "cpSpace", cpSpace_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpSpace");			 /* create metatable for cpSpace, and add it to the Lua registry */
  luaL_openlib(L, 0, cpSpace_meta, 0);	 /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);					/* dup methods table*/
  lua_rawset(L, -3);						/* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);					/* dup methods table*/
  lua_rawset(L, -3);						/* hide metatable:
													  metatable.__metatable = methods */
  lua_pop(L, 2);							 /* drop metatable */
  
  return 0;									/* return methods on the stack */
}



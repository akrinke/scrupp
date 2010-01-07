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

#include "cpVect-lua.h"
#include "cpBody-lua.h"
#include "cpMisc-lua.h"
#include "cpConstraint-lua.h"

#include <string.h> // for callback function name copy (will change)

cpSpace *push_cpSpace (lua_State *L) {
   cpSpace **ptr = (cpSpace **)lua_newuserdata(L, sizeof(cpSpace*));
   *ptr = cpSpaceAlloc();
	luaL_getmetatable(L, "cpSpace");
	lua_setmetatable(L, -2);
   return *ptr;
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
	cpSpace *s = push_cpSpace(L); // have to allocate onto stack
	cpSpaceInit(s);		  // so initialise it manually
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
	
typedef struct {
	void *udata;
	char functionName[128];
	lua_State *callbackState;
} callbackData;

int CollFunc(cpShape *a, cpShape *b, cpContact *contacts, int numContacts, cpFloat normal_coef, void *data) {
	callbackData* cbd=(callbackData*)data;
	lua_State *L=cbd->callbackState;
	lua_getglobal(L,cbd->functionName); // function name to function ref on stack

	lua_pushliteral(L, "werechip.cpShape_ptrs");
	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L,a); // userdata for shapes indexed by c pointer
	lua_rawget(L,-2);
	lua_remove (L, -2); // we want the ud ref from the table but not the table on the stack

	lua_pushliteral(L, "werechip.cpShape_ptrs");
	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L,b); // shape b
	lua_rawget(L,-2);
	lua_remove (L, -2); 
	
	lua_pushlightuserdata(L,contacts);
	lua_pushinteger(L,numContacts);
	lua_pushnumber(L,normal_coef);
	
	int e=lua_pcall(L, 5, 1, 0);
	if (e!=0) {
		printf("callback Lua error: %s\n",lua_tostring(L,-1));
		lua_pop(L,1);
		return 1;	
	}

	int r=lua_toboolean(L,1);
	lua_pop(L,1);

	return r;
}

//void cpSpaceAddCollisionPairFunc(cpSpace *space, cpCollisionType a, cpCollisionType b, cpCollFunc func, void *data) 
static int cpSpace_addCollisionPairFunc(lua_State *L) {
	cpSpace *space = check_cpSpace(L,1);
	unsigned int a=luaL_checkinteger(L,2);
	unsigned int b=luaL_checkinteger(L,3);
	char* func=(char*)lua_tostring(L,4); 
	callbackData *cbdata=malloc(sizeof(callbackData)); // TODO TODO free this when coll pair func removed  TODO TODO
	cbdata->callbackState=L;
	strcpy(cbdata->functionName,func);
//	cbdata->udata=data;  TODO add lua param for userdata...
	cpSpaceAddCollisionPairFunc(space, a,b, CollFunc , cbdata) ;
	return 0;
}

/*
typedef struct cpContact{
		  // Contact point and normal.
		  cpVect p, n;
		  // Penetration distance.
		  cpFloat dist;
		  ...
} cpContact;
*/
static int cpSpace_getContactPoint(lua_State *L) {
	cpContact *c;
	c=(cpContact*)lua_touserdata(L,2); // start at stack index 2 as we dont need cpSpace...
	int n=luaL_checkinteger(L,3);
	cpVect *pos  = push_cpVect(L);
	pos->x = c[n].p.x;
	pos->y = c[n].p.y;
	return 1;
}

static int cpSpace_getContactNormal(lua_State *L) {
	cpContact *c;
	c=(cpContact*)lua_touserdata(L,2); // start at stack index 2 as we dont need space...
	int n=luaL_checkinteger(L,3);
	cpVect *norm  = push_cpVect(L);
	norm->x = c[n].n.x;
	norm->y = c[n].n.y;
	return 1;	
}

static int cpSpace_getContactDistance(lua_State *L) {
	cpContact *c;
	c=(cpContact*)lua_touserdata(L,2); // start at stack index 2 as we dont need space...
	int n=luaL_checkinteger(L,3);
	lua_pushnumber(L,c[n].dist);
	return 1;		
}

static const luaL_reg cpSpace_methods[] = {
	{"new",							cpSpace_new},
	{"addShape",					cpSpace_addShape},
	{"addStaticShape",			cpSpace_addStaticShape},
	{"addBody",						cpSpace_addBody},
	{"addConstraint",				cpSpace_addConstraint},
	{"step",							cpSpace_step},
	{"setGravity",					cpSpace_setGravity},
	{"addCollisionPairFunc",	cpSpace_addCollisionPairFunc},
	{"getContactPoint",			cpSpace_getContactPoint},
	{"getContactNormal",			cpSpace_getContactNormal},
	{"getContactDistance",		cpSpace_getContactDistance},
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



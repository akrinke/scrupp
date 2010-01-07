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

#include "cpVect-lua.h"
#include "cpMisc-lua.h"



static cpBody *push_cpBody (lua_State *L) {
   cpBody *bb=cpBodyNew(0,0); // initialise later
   cpBody **pbb = (cpBody **)lua_newuserdata(L, sizeof(cpBody*));
   *pbb = bb;

   luaL_getmetatable(L, "cpBody");
   lua_setmetatable(L, -2);

   lua_pushliteral(L, "werechip.cpBody_ptrs");
   lua_gettable(L, LUA_REGISTRYINDEX);
   lua_pushlightuserdata(L, bb);   // table index
   lua_pushvalue(L,-3); // previously created table of *cpBody pointers to userdata
   lua_rawset(L, -3); // update the table
   lua_pop(L,1);

  return bb;
}

cpBody* check_cpBody (lua_State *L, int index) {
  cpBody *bb;
  luaL_checktype(L, index, LUA_TUSERDATA);
  cpBody **pbb =(cpBody**)luaL_checkudata(L, index, "cpBody");
  bb=*pbb;
  if (bb == NULL) luaL_typerror(L, index, "cpBody");
  return (cpBody*)bb;
}

static int cpBody_newStatic(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 0 ) return luaL_error(L, "Got %d arguments expected none", n);

   cpBody *b = push_cpBody(L); // have to allocate onto stack
   cpBodyInit(b, 1e100, 1e100);        // so initialise it manually
   
   return 1;
}

static int cpBody_new(lua_State *L) {

   int n = lua_gettop(L);  // Number of arguments
   
   if (n != 2 ) return luaL_error(L, "Got %d arguments expected 2, mass and inertia", n);
   
   double m = luaL_checknumber (L, 1);      
   double i = luaL_checknumber (L, 2);

   cpBody *b = push_cpBody(L); // have to allocate onto stack
   cpBodyInit(b, m, i);        // so initialise it manually
   
   return 1;
}

static int cpBody_setMass (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   double m = luaL_checknumber(L, 2);
   cpBodySetMass(b,m);
   return 0;
}   

static int cpBody_setMoment (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   double i = luaL_checknumber(L, 2);
   cpBodySetMoment(b,i);
   return 0;
}  

static int cpBody_setAngle (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   double a = luaL_checknumber(L, 2);
   cpBodySetAngle(b,a);
   return 0;
}

static int cpBody_setPos (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   cpVect *v = check_cpVect(L, 2);
   cpBodySetPos(b,*v);
   return 0;
}   

static int cpBody_setForce (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   cpVect *v = check_cpVect(L, 2);
   cpBodySetForce(b,*v);
   return 0;
}   

static int cpBody_setVel (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   cpVect *v = check_cpVect(L, 2);
   cpBodySetVel(b,*v);
   return 0;
}   

static int cpBody_setAngVel (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   double av = luaL_checknumber(L, 2);
   cpBodySetAngVel(b,av);
   return 0;
}

static int cpBody_setTorque (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   double t = luaL_checknumber(L, 2);
   cpBodySetTorque(b,t);
   return 0;
}

static int cpBody_getMoment (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   lua_pushnumber(L, cpBodyGetMoment(b));
   return 1;
}

static int cpBody_getAngle (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   lua_pushnumber(L, cpBodyGetAngle(b));
   return 1;
}

static int cpBody_getAngVel (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   lua_pushnumber(L, cpBodyGetAngVel(b));
   return 1;
}

static int cpBody_getTorque (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   lua_pushnumber(L, cpBodyGetTorque(b));
   return 1;
}

static int cpBody_getMass (lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   lua_pushnumber(L, cpBodyGetMass(b));
   return 1;
}

static int cpBody_getPos (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *v  = push_cpVect(L);
   cpVect vi=cpBodyGetPos(b);
   v->x = vi.x;
   v->y = vi.y;
   return 1;
}

static int cpBody_getVel (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *v  = push_cpVect(L);
   cpVect vi=cpBodyGetVel(b);
   v->x = vi.x;
   v->y = vi.y;
   return 1;
}

static int cpBody_getForce (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *v  = push_cpVect(L);
   cpVect vi=cpBodyGetPos(b);
   v->x = vi.x;
   v->y = vi.y;
   return 1;
}

static int cpBody_getRot (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *v  = push_cpVect(L);
   cpVect vi=cpBodyGetRot(b);
   v->x = vi.x;
   v->y = vi.y;
   return 1;
}

static int cpBody_getLocal2World (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *vi=check_cpVect(L,2);
   cpVect *v  = push_cpVect(L);
   cpVect vo=cpBodyLocal2World(b,*vi);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}

static int cpBody_getWorld2Local (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *vi=check_cpVect(L,2);
   cpVect *v  = push_cpVect(L);
   cpVect vo=cpBodyWorld2Local(b,*vi);
   v->x = vo.x;
   v->y = vo.y;
   return 1;
}

static int cpBody_ApplyImpulse (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *j = check_cpVect(L, 2);
   cpVect *r = check_cpVect(L, 3);
   cpBodyApplyImpulse(b,*j,*r);
   return 0;
}

static int cpBody_ResetForces (lua_State *L){ 
   cpBody *b = check_cpBody(L, 1);
   cpBodyResetForces(b);
   return 0;
}

static int cpBody_ApplyForce (lua_State *L){
   cpBody *b = check_cpBody(L, 1);
   cpVect *f = check_cpVect(L, 2);
   cpVect *r = check_cpVect(L, 3);
   cpBodyApplyForce(b,*f,*r);
   return 0;
}

// depricated!!!
//void cpApplyDampedSpring(cpBody *a, cpBody *b, cpVect anchr1, cpVect anchr2, cpFloat rlen, cpFloat k, cpFloat dmp, cpFloat dt)
static int cpBody_ApplyDampedSpring (lua_State *L){
   cpBody *a = check_cpBody(L, 1);
   cpBody *b = check_cpBody(L, 2);
   cpVect *an1 = check_cpVect(L, 3);
   cpVect *an2 = check_cpVect(L, 4);
   double rlen = luaL_checknumber(L,5);
   double k = luaL_checknumber(L,6);   
   double dmp = luaL_checknumber(L,7); 
   double dt = luaL_checknumber(L,8);    
   cpApplyDampedSpring(a, b, *an1, *an2, rlen, k, dmp, dt);
   return 0;
}

static const luaL_reg cpBody_methods[] = {
  {"new",               cpBody_new},
  {"newStatic",         cpBody_newStatic},
  {"setMass",           cpBody_setMass},
  {"setMoment",         cpBody_setMoment},
  {"setAngle",          cpBody_setAngle},
  {"setPos",            cpBody_setPos},
  {"setForce",          cpBody_setForce},
  {"setAngVel",         cpBody_setAngVel},
  {"setTorque",         cpBody_setTorque},
  {"getMass",           cpBody_getMass},
  {"getMoment",         cpBody_getMoment},
  {"getAngle",          cpBody_getAngle},
  {"getAngVel",         cpBody_getAngVel},
  {"getTorque",         cpBody_getTorque},
  {"getPos",            cpBody_getPos},
  {"getVel",            cpBody_getVel},
  {"setVel",            cpBody_setVel},
  {"getForce",          cpBody_getForce},
  {"getRot",            cpBody_getRot},
  {"getLocal2World",    cpBody_getLocal2World},
  {"getWorld2Local",    cpBody_getWorld2Local},
  {"applyImpulse",      cpBody_ApplyImpulse},
  {"resetForces",       cpBody_ResetForces},
  {"applyForce",        cpBody_ApplyForce},
  {"applyDampedSpring", cpBody_ApplyDampedSpring},
  {0, 0}
};  
  
static int cpBody_tostring (lua_State *L) {
   // TODO eventually provide text of xml tag representing
   // this object and its properties
   char buff[32];
   sprintf(buff,"%p->%p",lua_touserdata(L,1),deref(lua_touserdata(L,1)));
   lua_pushfstring(L, "cpBody (%s)", buff);
   return 1;
}

static int cpBody_gc(lua_State *L) {
   cpBody *b = check_cpBody(L, 1);
   cpBodyFree(b);
   return 0;
}

static const luaL_reg cpBody_meta[] = {  
   {"__tostring", cpBody_tostring}, 
   {"__gc",       cpBody_gc}, // only holding pointer...
   {0, 0}                        // as Lua GC is freeing it, well thats the theory.... :D
};

int cpBody_register (lua_State *L) {
  luaL_openlib(L, "cpBody", cpBody_methods, 0);  /* create methods table, add it to the globals */
  luaL_newmetatable(L, "cpBody");          /* create metatable for cpBody, and add it to the Lua registry */
  luaL_openlib(L, 0, cpBody_meta, 0);    /* fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 2);                      /* drop metatable */
  
  /* create a table with weak values which maps C pointers (light userdata) to full userdata */
  lua_pushliteral(L, "werechip.cpBody_ptrs");
  lua_newtable(L);
  lua_pushvalue(L, -1);               /* duplicate the table */
  lua_pushliteral(L, "__mode");
  lua_pushliteral(L, "v");
  lua_rawset(L, -3);                  /* table.__mode = 'v' */
  lua_setmetatable(L, -2);            /* table.metatable = table */
  lua_settable(L, LUA_REGISTRYINDEX);

  return 0;                           /* return methods on the stack */
}

  

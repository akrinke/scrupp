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

#ifndef CPSHAPE_LUA_H
#define CPSHAPE_LUA_H

#define DEFINE_SHAPE_METHODS \
  {"getBody", cpShape_getBody}, \
  {"setBody", cpShape_setBody}, \
  {"isSensor", cpShape_isSensor}, \
  {"setSensor", cpShape_setSensor}, \
  {"getCollisionType", cpShape_getCollisionType }, \
  {"setCollisionType", cpShape_setCollisionType }, \
  {"getGroup", cpShape_getGroup}, \
  {"setGroup", cpShape_setGroup}, \
  {"getLayers", cpShape_getLayers}, \
  {"setLayers", cpShape_setLayers}, \
  {"getBB", cpShape_getBB}, \
  {"cacheBB", cpShape_cacheBB}, \
  {"getElasticity", cpShape_getElasticity}, \
  {"setElasticity", cpShape_setElasticity}, \
  {"getFriction", cpShape_getFriction}, \
  {"setFriction", cpShape_setFriction}, \
  {"getSurfaceVelocity", cpShape_getSurfaceVelocity}, \
  {"setSurfaceVelocity", cpShape_setSurfaceVelocity}

cpShape *check_cpShape (lua_State *L, int index);
int cpShape_getBody (lua_State *L);
int cpShape_setBody (lua_State *L);
int cpShape_isSensor (lua_State *L);
int cpShape_setSensor (lua_State *L);
int cpShape_getCollisionType (lua_State *L);
int cpShape_setCollisionType (lua_State *L);
int cpShape_getGroup (lua_State *L);
int cpShape_setGroup (lua_State *L);
int cpShape_getLayers (lua_State *L);
int cpShape_setLayers (lua_State *L);
int cpShape_getBB (lua_State *L);
int cpShape_cacheBB (lua_State *L);
int cpShape_getElasticity (lua_State *L);
int cpShape_setElasticity (lua_State *L);
int cpShape_getFriction (lua_State *L);
int cpShape_setFriction (lua_State *L);
int cpShape_getSurfaceVelocity (lua_State *L);
int cpShape_setSurfaceVelocity (lua_State *L);
int cpShape_gc (lua_State *L);

#endif

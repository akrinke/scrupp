static int
path_gc (lua_State *L) {
    cairo_path_t **obj = luaL_checkudata(L, 1, MT_NAME_PATH);
    cairo_path_destroy(*obj);
    *obj = 0;
    return 0;
}

static int
path_each_iter (lua_State *L) {
    cairo_path_t *path = *(cairo_path_t **) luaL_checkudata(L, 1, MT_NAME_PATH);
    cairo_path_data_t *data;
    int i;

    if (lua_isnoneornil(L, 2))
        i = 0;
    else {
        i = luaL_checkint(L, 2);
        luaL_argcheck(L, i >= 0 && i < path->num_data, 2,
                      "path index out of range");
        i += path->data[i].header.length;
    }
    if (i >= path->num_data)
        return 0;

    lua_pushinteger(L, i);
    data = &path->data[i];
    switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            lua_pushliteral(L, "move-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            break;
        case CAIRO_PATH_LINE_TO:
            lua_pushliteral(L, "line-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            break;
        case CAIRO_PATH_CURVE_TO:
            lua_pushliteral(L, "curve-to");
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, data[1].point.x);
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, data[1].point.y);
            lua_rawseti(L, -2, 2);
            lua_pushnumber(L, data[2].point.x);
            lua_rawseti(L, -2, 3);
            lua_pushnumber(L, data[2].point.y);
            lua_rawseti(L, -2, 4);
            lua_pushnumber(L, data[3].point.x);
            lua_rawseti(L, -2, 5);
            lua_pushnumber(L, data[3].point.y);
            lua_rawseti(L, -2, 6);
            break;
        case CAIRO_PATH_CLOSE_PATH:
            lua_pushliteral(L, "close-path");
            lua_pushnil(L);
            break;
        default:
            assert(0);
    }
    return 3;
}

static int
path_each (lua_State *L) {
    luaL_checkudata(L, 1, MT_NAME_PATH);
    lua_pushcfunction(L, path_each_iter);
    lua_pushvalue(L, 1);
    return 2;
}

static const luaL_Reg
path_methods[] = {
    { "__gc", path_gc },
    { "each", path_each },
    { 0, 0 }
};

/* vi:set ts=4 sw=4 expandtab: */

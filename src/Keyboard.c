/*
** $Id$
** Library to use the keyboard with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Keyboard.h"

#include <SDL_keysym.h>

#define E(name, val) \
  lua_pushliteral(L, name); \
  lua_pushinteger(L, val); \
  lua_rawset(L, -3); \
  lua_pushliteral(L, name); \
  lua_rawseti(L, -2, val);

static int Lua_Keyboard_keyIsDown(lua_State *L) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
	Uint16 key;
	
	luaL_argcheck(L, lua_tostring(L, -1) != NULL, 1, "string expected");
	lua_pushliteral(L, "Scrupp:key_table");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	lua_rawget(L, -2);
	luaL_argcheck(L, !lua_isnil(L, -1), 1, "unknown key");
	key = (Uint16) lua_tointeger(L, -1);
	
	if (key>1000) {
		key = key - 1000;
		lua_pushboolean(L, keystate[key] + keystate[key+1]);
	} else
		lua_pushboolean(L, keystate[key]);
	return 1;
}

static int Lua_Keyboard_setKeyRepeat(lua_State *L) {
	int delay = luaL_checkint(L, 1);
	int interval = luaL_checkint(L, 2);
	SDL_EnableKeyRepeat(delay, interval);
	return 0;
}

static const struct luaL_Reg keyboardlib [] = {
	{"keyIsDown", Lua_Keyboard_keyIsDown},
	{"setKeyRepeat", Lua_Keyboard_setKeyRepeat},
	{NULL, NULL}
};

int luaopen_keyboard(lua_State *L, const char *parent) {
	luaL_register(L, parent, keyboardlib);	/* leaves table on top of stack */

	/*	create a table in the registry containing the mappings from
		key names to SDLK numbers and vice versa */
	lua_newtable(L);
	
	/* 	at first: generate virtual entries for SHIFT, CTRL, ALT, META and SUPER
		these combine the left and right key
		this way you can write scrupp.keyIsDown("SHIFT") and test 
		for LSHIFT and RSHIFT */

	E("SHIFT", 1000 + SDLK_RSHIFT);
	E("CTRL", 1000 + SDLK_RCTRL);
	E("ALT", 1000 + SDLK_RALT);
	E("META", 1000 + SDLK_RMETA);
	E("SUPER", 1000 + SDLK_LSUPER);

	/* this generates table entries for every key in SDL_keysym.h */
	E("UNKNOWN", SDLK_UNKNOWN);
	E("BACKSPACE", SDLK_BACKSPACE);
	E("TAB", SDLK_TAB);
	E("CLEAR", SDLK_CLEAR);
	E("RETURN", SDLK_RETURN);
	E("PAUSE", SDLK_PAUSE);
	E("ESCAPE", SDLK_ESCAPE);
	E("SPACE", SDLK_SPACE);
	E("!", SDLK_EXCLAIM);
	E("\"", SDLK_QUOTEDBL);
	E("#", SDLK_HASH);
	E("$", SDLK_DOLLAR);
	E("&", SDLK_AMPERSAND);
	E("'", SDLK_QUOTE);
	E("(", SDLK_LEFTPAREN);
	E(")", SDLK_RIGHTPAREN);
	E("*", SDLK_ASTERISK);
	E("+", SDLK_PLUS);
	E(",", SDLK_COMMA);
	E("-", SDLK_MINUS);
	E(".", SDLK_PERIOD);
	E("/", SDLK_SLASH);
	E("0", SDLK_0);
	E("1", SDLK_1);
	E("2", SDLK_2);
	E("3", SDLK_3);
	E("4", SDLK_4);
	E("5", SDLK_5);
	E("6", SDLK_6);
	E("7", SDLK_7);
	E("8", SDLK_8);
	E("9", SDLK_9);
	E(":", SDLK_COLON);
	E(";", SDLK_SEMICOLON);
	E("<", SDLK_LESS);
	E("=", SDLK_EQUALS);
	E(">", SDLK_GREATER);
	E("?", SDLK_QUESTION);
	E("@", SDLK_AT);
	/*
	   Skip uppercase letters
	 */
	E("[", SDLK_LEFTBRACKET);
	E("\\", SDLK_BACKSLASH);
	E("]", SDLK_RIGHTBRACKET);
	E("^", SDLK_CARET);
	E("_", SDLK_UNDERSCORE);
	E("`", SDLK_BACKQUOTE);
	E("a", SDLK_a);
	E("b", SDLK_b);
	E("c", SDLK_c);
	E("d", SDLK_d);
	E("e", SDLK_e);
	E("f", SDLK_f);
	E("g", SDLK_g);
	E("h", SDLK_h);
	E("i", SDLK_i);
	E("j", SDLK_j);
	E("k", SDLK_k);
	E("l", SDLK_l);
	E("m", SDLK_m);
	E("n", SDLK_n);
	E("o", SDLK_o);
	E("p", SDLK_p);
	E("q", SDLK_q);
	E("r", SDLK_r);
	E("s", SDLK_s);
	E("t", SDLK_t);
	E("u", SDLK_u);
	E("v", SDLK_v);
	E("w", SDLK_w);
	E("x", SDLK_x);
	E("y", SDLK_y);
	E("z", SDLK_z);
	E("DELETE", SDLK_DELETE);
	/* End of ASCII mapped keysyms */
	
	/* International keyboard syms */
	E("WORLD_0", SDLK_WORLD_0);
	E("WORLD_1", SDLK_WORLD_1);
	E("WORLD_2", SDLK_WORLD_2);
	E("WORLD_3", SDLK_WORLD_3);
	E("WORLD_4", SDLK_WORLD_4);
	E("WORLD_5", SDLK_WORLD_5);
	E("WORLD_6", SDLK_WORLD_6);
	E("WORLD_7", SDLK_WORLD_7);
	E("WORLD_8", SDLK_WORLD_8);
	E("WORLD_9", SDLK_WORLD_9);
	E("WORLD_10", SDLK_WORLD_10);
	E("WORLD_11", SDLK_WORLD_11);
	E("WORLD_12", SDLK_WORLD_12);
	E("WORLD_13", SDLK_WORLD_13);
	E("WORLD_14", SDLK_WORLD_14);
	E("WORLD_15", SDLK_WORLD_15);
	E("WORLD_16", SDLK_WORLD_16);
	E("WORLD_17", SDLK_WORLD_17);
	E("WORLD_18", SDLK_WORLD_18);
	E("WORLD_19", SDLK_WORLD_19);
	E("WORLD_20", SDLK_WORLD_20);
	E("WORLD_21", SDLK_WORLD_21);
	E("WORLD_22", SDLK_WORLD_22);
	E("WORLD_23", SDLK_WORLD_23);
	E("WORLD_24", SDLK_WORLD_24);
	E("WORLD_25", SDLK_WORLD_25);
	E("WORLD_26", SDLK_WORLD_26);
	E("WORLD_27", SDLK_WORLD_27);
	E("WORLD_28", SDLK_WORLD_28);
	E("WORLD_29", SDLK_WORLD_29);
	E("WORLD_30", SDLK_WORLD_30);
	E("WORLD_31", SDLK_WORLD_31);
	E("WORLD_32", SDLK_WORLD_32);
	E("WORLD_33", SDLK_WORLD_33);
	E("WORLD_34", SDLK_WORLD_34);
	E("WORLD_35", SDLK_WORLD_35);
	E("WORLD_36", SDLK_WORLD_36);
	E("WORLD_37", SDLK_WORLD_37);
	E("WORLD_38", SDLK_WORLD_38);
	E("WORLD_39", SDLK_WORLD_39);
	E("WORLD_40", SDLK_WORLD_40);
	E("WORLD_41", SDLK_WORLD_41);
	E("WORLD_42", SDLK_WORLD_42);
	E("WORLD_43", SDLK_WORLD_43);
	E("WORLD_44", SDLK_WORLD_44);
	E("WORLD_45", SDLK_WORLD_45);
	E("WORLD_46", SDLK_WORLD_46);
	E("WORLD_47", SDLK_WORLD_47);
	E("WORLD_48", SDLK_WORLD_48);
	E("WORLD_49", SDLK_WORLD_49);
	E("WORLD_50", SDLK_WORLD_50);
	E("WORLD_51", SDLK_WORLD_51);
	E("WORLD_52", SDLK_WORLD_52);
	E("WORLD_53", SDLK_WORLD_53);
	E("WORLD_54", SDLK_WORLD_54);
	E("WORLD_55", SDLK_WORLD_55);
	E("WORLD_56", SDLK_WORLD_56);
	E("WORLD_57", SDLK_WORLD_57);
	E("WORLD_58", SDLK_WORLD_58);
	E("WORLD_59", SDLK_WORLD_59);
	E("WORLD_60", SDLK_WORLD_60);
	E("WORLD_61", SDLK_WORLD_61);
	E("WORLD_62", SDLK_WORLD_62);
	E("WORLD_63", SDLK_WORLD_63);
	E("WORLD_64", SDLK_WORLD_64);
	E("WORLD_65", SDLK_WORLD_65);
	E("WORLD_66", SDLK_WORLD_66);
	E("WORLD_67", SDLK_WORLD_67);
	E("WORLD_68", SDLK_WORLD_68);
	E("WORLD_69", SDLK_WORLD_69);
	E("WORLD_70", SDLK_WORLD_70);
	E("WORLD_71", SDLK_WORLD_71);
	E("WORLD_72", SDLK_WORLD_72);
	E("WORLD_73", SDLK_WORLD_73);
	E("WORLD_74", SDLK_WORLD_74);
	E("WORLD_75", SDLK_WORLD_75);
	E("WORLD_76", SDLK_WORLD_76);
	E("WORLD_77", SDLK_WORLD_77);
	E("WORLD_78", SDLK_WORLD_78);
	E("WORLD_79", SDLK_WORLD_79);
	E("WORLD_80", SDLK_WORLD_80);
	E("WORLD_81", SDLK_WORLD_81);
	E("WORLD_82", SDLK_WORLD_82);
	E("WORLD_83", SDLK_WORLD_83);
	E("WORLD_84", SDLK_WORLD_84);
	E("WORLD_85", SDLK_WORLD_85);
	E("WORLD_86", SDLK_WORLD_86);
	E("WORLD_87", SDLK_WORLD_87);
	E("WORLD_88", SDLK_WORLD_88);
	E("WORLD_89", SDLK_WORLD_89);
	E("WORLD_90", SDLK_WORLD_90);
	E("WORLD_91", SDLK_WORLD_91);
	E("WORLD_92", SDLK_WORLD_92);
	E("WORLD_93", SDLK_WORLD_93);
	E("WORLD_94", SDLK_WORLD_94);
	E("WORLD_95", SDLK_WORLD_95);

	/* Numeric keypad */
	E("KP0", SDLK_KP0);
	E("KP1", SDLK_KP1);
	E("KP2", SDLK_KP2);
	E("KP3", SDLK_KP3);
	E("KP4", SDLK_KP4);
	E("KP5", SDLK_KP5);
	E("KP6", SDLK_KP6);
	E("KP7", SDLK_KP7);
	E("KP8", SDLK_KP8);
	E("KP9", SDLK_KP9);
	E("KP_PERIOD", SDLK_KP_PERIOD);
	E("KP_DIVIDE", SDLK_KP_DIVIDE);
	E("KP_MULTIPLY", SDLK_KP_MULTIPLY);
	E("KP_MINUS", SDLK_KP_MINUS);
	E("KP_PLUS", SDLK_KP_PLUS);
	E("KP_ENTER", SDLK_KP_ENTER);
	E("KP_EQUALS", SDLK_KP_EQUALS);

	/* Arrows + Home/End pad */
	E("UP", SDLK_UP);
	E("DOWN", SDLK_DOWN);
	E("RIGHT", SDLK_RIGHT);
	E("LEFT", SDLK_LEFT);
	E("INSERT", SDLK_INSERT);
	E("HOME", SDLK_HOME);
	E("END", SDLK_END);
	E("PAGEUP", SDLK_PAGEUP);
	E("PAGEDOWN", SDLK_PAGEDOWN);

	/* Function keys */
	E("F1", SDLK_F1);
	E("F2", SDLK_F2);
	E("F3", SDLK_F3);
	E("F4", SDLK_F4);
	E("F5", SDLK_F5);
	E("F6", SDLK_F6);
	E("F7", SDLK_F7);
	E("F8", SDLK_F8);
	E("F9", SDLK_F9);
	E("F10", SDLK_F10);
	E("F11", SDLK_F11);
	E("F12", SDLK_F12);
	E("F13", SDLK_F13);
	E("F14", SDLK_F14);
	E("F15", SDLK_F15);

	/* Key state modifier keys */
	E("NUMLOCK", SDLK_NUMLOCK);
	E("CAPSLOCK", SDLK_CAPSLOCK);
	E("SCROLLOCK", SDLK_SCROLLOCK);
	E("RSHIFT", SDLK_RSHIFT);
	E("LSHIFT", SDLK_LSHIFT);
	E("RCTRL", SDLK_RCTRL);
	E("LCTRL", SDLK_LCTRL);
	E("RALT", SDLK_RALT);
	E("LALT", SDLK_LALT);
	E("RMETA", SDLK_RMETA);
	E("LMETA", SDLK_LMETA);
	E("LSUPER", SDLK_LSUPER);
	E("RSUPER", SDLK_RSUPER);
	E("MODE", SDLK_MODE);
	E("COMPOSE", SDLK_COMPOSE);

	/* Miscellaneous function keys */
	E("HELP", SDLK_HELP);
	E("PRINT", SDLK_PRINT);
	E("SYSREQ", SDLK_SYSREQ);
	E("BREAK", SDLK_BREAK);
	E("MENU", SDLK_MENU);
	E("POWER", SDLK_POWER);
	E("EURO", SDLK_EURO);
	E("UNDO", SDLK_UNDO);
	
	lua_setfield(L, LUA_REGISTRYINDEX, "Scrupp:key_table");

	return 1;
}

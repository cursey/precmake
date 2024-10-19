#pragma once

#include "lua.h"
#include "types.h"

void api_init_and_equip(lua_State *lua);
void api_release(lua_State *lua);
Project *api_get_project(lua_State *lua);

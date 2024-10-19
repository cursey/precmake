#pragma once

#include "utl/utl.h"
#include "lua.h"
#include "types.h"

typedef struct Ud Ud;
struct Ud
{
    Arena *arena;
    void *value;
};

Ud *ud_condition_new(Arena *arena, lua_State *lua);
Ud *ud_condition_get(lua_State *lua, int index);
Ud *ud_condition_test(lua_State *lua, int index);
Condition *ud_condition_get_or_make(Arena *arena, lua_State *lua, int index);
Ud *ud_property_new(Arena *arena, lua_State *lua);
Ud *ud_property_get(lua_State *lua, int index);
Ud *ud_target_new(Arena *arena, lua_State *lua);
Ud *ud_target_get(lua_State *lua, int index);
Ud *ud_glob_exclusion_new(Arena *arena, lua_State *lua);
Ud *ud_glob_exclusion_get(lua_State *lua, int index);
Ud *ud_glob_new(Arena *arena, lua_State *lua);
Ud *ud_glob_get(lua_State *lua, int index);
Ud *ud_when_new(Arena *arena, lua_State *lua);
Ud *ud_when_get(lua_State *lua, int index);
Ud *ud_custom_command_new(Arena *arena, lua_State *lua);
Ud *ud_custom_command_get(lua_State *lua, int index);
Ud *ud_option_new(Arena *arena, lua_State *lua);
Ud *ud_option_get(lua_State *lua, int index);
Ud *ud_option_test(lua_State *lua, int index);
Ud *ud_package_new(Arena *arena, lua_State *lua);
Ud *ud_package_get(lua_State *lua, int index);
Ud *ud_project_new(Arena *arena, lua_State *lua);
Ud *ud_project_get(lua_State *lua, int index);
void ud_init_and_equip(lua_State *lua);

#include <stdio.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "utl/utl.h"
#include "api.h"
#include "gen.h"
#include "types.h"

static lua_State *precmake_init_lua()
{
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);
    api_init_and_equip(lua);
    return lua;
}

static void precmake_close_lua(lua_State *lua)
{
    api_release(lua);
    lua_close(lua);
}

static void precmake_do_file(lua_State *lua, const char *filepath)
{
    if (luaL_dofile(lua, filepath) != 0)
    {
        printf("Error: %s\n", lua_tostring(lua, -1));
        return;
    }
    Project *project = api_get_project(lua);
    Temp scratch = scratch_begin(0, 0);
    Str output = gen_cmakelists(scratch.arena, project);
    printf("%.*s\n", str_varg(output));
    scratch_end(scratch);
}

int main(int argc, char *argv[])
{
    ThreadLocals locals;
    thread_locals_init_and_equip(&locals);
    lua_State *lua = precmake_init_lua();
    const char *filepath = "precmake.lua";
    if (argc > 1)
    {
        filepath = argv[1];
    }
    precmake_do_file(lua, filepath);
    precmake_close_lua(lua);
    thread_locals_release();
    return 0;
}
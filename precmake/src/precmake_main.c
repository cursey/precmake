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

static void test()
{
    Temp scratch = scratch_begin(0, 0);
    Arena *arena = scratch.arena;

    // Make a project.
    Project *cuttle = push_array(arena, Project, 1);
    cuttle->name = str_lit("cuttle");

    // Make a glob.
    Glob *utl_sources = push_array(arena, Glob, 1);
    utl_sources->name = str_lit("utl_sources");
    utl_sources->pattern = str_lit("utl/src/*.c");
    project_push_thing(arena, cuttle, ThingType_Glob, utl_sources);

    // Make a windows exclusion.
    Condition *not_win32 = push_array(arena, Condition, 1);
    condition_push_expression(arena, not_win32, str_lit("NOT WIN32"));
    ListOp *win32_exclusion = push_array(arena, ListOp, 1);
    // win32_exclusion->pattern = str_lit(".*win32.c$");
    list_op_push_item(arena, win32_exclusion, str_lit(".*win32.c$"));
    list_op_push_condition(win32_exclusion, not_win32);
    glob_push_operation(utl_sources, win32_exclusion);

    // Make an apple exclusion.
    Condition *not_apple = push_array(arena, Condition, 1);
    condition_push_expression(arena, not_apple, str_lit("NOT APPLE"));
    ListOp *apple_exclusion = push_array(arena, ListOp, 1);
    // apple_exclusion->pattern = str_lit(".*macos.c$");
    list_op_push_item(arena, apple_exclusion, str_lit(".*macos.c$"));
    list_op_push_condition(apple_exclusion, not_apple);
    glob_push_operation(utl_sources, apple_exclusion);

    // Make a linux exclusion.
    Condition *not_linux = push_array(arena, Condition, 1);
    condition_push_expression(arena, not_linux, str_lit("NOT (UNIX OR APPLE)"));
    ListOp *linux_exclusion = push_array(arena, ListOp, 1);
    // linux_exclusion->pattern = str_lit(".*linux.c$");
    list_op_push_item(arena, linux_exclusion, str_lit(".*linux.c$"));
    list_op_push_condition(linux_exclusion, not_linux);
    glob_push_operation(utl_sources, linux_exclusion);

    // Make a target.
    Target *utl = push_array(arena, Target, 1);
    utl->name = str_lit("utl");
    utl->type = TargetType_StaticLibrary;
    target_push_source(arena, utl, str_lit("${utl_sources}"));
    project_push_thing(arena, cuttle, ThingType_Target, utl);

    // Make another target.
    Glob *precmake_sources = push_array(arena, Glob, 1);
    precmake_sources->name = str_lit("precmake_sources");
    precmake_sources->pattern = str_lit("precmake/src/*.c");
    project_push_thing(arena, cuttle, ThingType_Glob, precmake_sources);
    Target *precmake = push_array(arena, Target, 1);
    precmake->name = str_lit("precmake");
    precmake->type = TargetType_Executable;
    target_push_source(arena, precmake, str_lit("${precmake_sources}"));
    Property *premake_links = push_array(arena, Property, 1);
    premake_links->visibility = PropertyVisibility_Private;
    premake_links->type = PropertyType_LinkLibraries;
    property_push_value(arena, premake_links, str_lit("utl"));
    property_push_value(arena, premake_links, str_lit("lua"));
    target_push_property(precmake, premake_links);
    project_push_thing(arena, cuttle, ThingType_Target, precmake);

    // Generate and print the result.
    Str output = gen_cmakelists(arena, cuttle);
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
    // test();
    precmake_close_lua(lua);
    thread_locals_release();
    return 0;
}
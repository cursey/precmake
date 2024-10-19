#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "utl/utl.h"
#include "types.h"
#include "ud.h"
#include "api.h"

static Arena *api_get_arena(lua_State *lua)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "arena");
    Arena *arena = lua_touserdata(lua, -1);
    lua_pop(lua, 1);
    return arena;
}

typedef struct ApiThingLists ApiThingLists;
struct ApiThingLists
{
    ThingList *thing_list;
    ApiThingLists *next;
    ApiThingLists *free;
};

static ThingList *api_thing_lists_top(lua_State *lua)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "thing_lists");
    if (lua_isnil(lua, -1))
    {
        Project *project = api_get_project(lua);
        lua_pop(lua, 1);

        // Create a new thing list stack and set it in the registry.
        Arena *arena = api_get_arena(lua);
        ApiThingLists *thing_lists = push_array(arena, ApiThingLists, 1);
        thing_lists->thing_list = &project->things;
        lua_pushlightuserdata(lua, thing_lists);
        lua_setfield(lua, LUA_REGISTRYINDEX, "thing_lists");
        return thing_lists->thing_list;
    }
    else
    {
        ApiThingLists *thing_lists = lua_touserdata(lua, -1);
        lua_pop(lua, 1);
        return thing_lists->thing_list;
    }
}

static void api_thing_lists_push(lua_State *lua, ThingList *thing_list)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "thing_lists");
    if (lua_isnil(lua, -1))
    {
        luaL_error(lua, "No thing list stack has been defined");
        return;
    }
    ApiThingLists *prev_node = lua_touserdata(lua, -1);
    ApiThingLists *node;
    lua_pop(lua, 1);
    if (prev_node->free)
    {
        node = prev_node->free;
        prev_node->free = node->next;
        return;
    }
    else
    {
        node = push_array(api_get_arena(lua), ApiThingLists, 1);
    }
    node->thing_list = thing_list;
    node->next = prev_node;
    lua_pushlightuserdata(lua, node);
    lua_setfield(lua, LUA_REGISTRYINDEX, "thing_lists");
}

static void api_thing_lists_pop(lua_State *lua)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "thing_lists");
    if (lua_isnil(lua, -1))
    {
        luaL_error(lua, "No thing list stack has been defined");
        return;
    }
    ApiThingLists *node = lua_touserdata(lua, -1);
    lua_pop(lua, 1);
    lua_pushlightuserdata(lua, node->next);
    lua_setfield(lua, LUA_REGISTRYINDEX, "thing_lists");
    node->next->free = node;
}

static Thing *api_thing_lists_push_thing(lua_State *lua, ThingType type, void *thing)
{
    Arena *arena = api_get_arena(lua);
    ThingList *thing_list = api_thing_lists_top(lua);
    return thing_list_push_thing(arena, thing_list, type, thing);
}

static int api_project(lua_State *lua)
{
    // Look at the registry to see if a project already exists (there can only be one).
    lua_getfield(lua, LUA_REGISTRYINDEX, "project");
    if (!lua_isnil(lua, -1))
    {
        return luaL_error(lua, "A project has already been defined");
    }
    lua_pop(lua, 1);

    // Create a new project and set it in the registry.
    Arena *arena = api_get_arena(lua);
    const char *name = luaL_checkstring(lua, 1);
    Project *project = ud_project_new(arena, lua)->value;
    project->name = push_str_copy(arena, str_from_cstr((u8 *)name));
    lua_pushvalue(lua, -1);
    lua_setfield(lua, LUA_REGISTRYINDEX, "project");
    // api_thing_lists_push(lua, &project->things);
    api_get_project(lua);
    return 1;
}

static int add_target(lua_State *lua, TargetType target_type)
{
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    const char *name = luaL_checkstring(lua, 1);
    luaL_checktype(lua, 2, LUA_TTABLE);
    Target *target = ud_target_new(arena, lua)->value;
    target->name = push_str_copy(arena, str_from_cstr((u8 *)name));
    target->type = target_type;
    for (int i = 1; i <= lua_rawlen(lua, 2); i++)
    {
        lua_rawgeti(lua, 2, i);
        switch (lua_type(lua, -1))
        {
        case LUA_TSTRING: {
            const char *source = luaL_checkstring(lua, -1);
            target_push_source(arena, target, push_str_copy(arena, str_from_cstr((u8 *)source)));
            break;
        }
        case LUA_TUSERDATA: {
            Glob *glob = ud_glob_get(lua, -1)->value;
            target_push_source(arena, target, push_strf(arena, "${%.*s}", str_varg(glob->name)));
            break;
        }
        }
        lua_pop(lua, 1);
    }
    // project_push_target(project, target);
    target->thing = api_thing_lists_push_thing(lua, ThingType_Target, target);
    return 1;
}

// local target = executable("name", {"src1.c", "src2.c"})
static int api_executable(lua_State *lua)
{
    return add_target(lua, TargetType_Executable);
}

// local target = static_library("name", {"src1.c", "src2.c"})
static int api_static_library(lua_State *lua)
{
    return add_target(lua, TargetType_StaticLibrary);
}

// local target = shared_library("name", {"src1.c", "src2.c"})
static int api_shared_library(lua_State *lua)
{
    return add_target(lua, TargetType_SharedLibrary);
}

// local target = custom_target("name", {"src1.c", "src2.c"})
static int api_custom_target(lua_State *lua)
{
    return add_target(lua, TargetType_Custom);
}

// local command = custom_command{
//     depends = {"src1.c", "src2.c"},
//     output = {"output1.c", "output2.c"},
//     command = "command",
//     comment = "comment",
// }
static int api_custom_command(lua_State *lua)
{
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    luaL_checktype(lua, 1, LUA_TTABLE);
    CustomCommand *command = ud_custom_command_new(arena, lua)->value;
    for (lua_pushnil(lua); lua_next(lua, 1); lua_pop(lua, 1))
    {
        const char *key = luaL_checkstring(lua, -2);
        if (strcmp(key, "depends") == 0)
        {
            luaL_checktype(lua, -1, LUA_TTABLE);
            for (int i = 1; i <= lua_rawlen(lua, -1); i++)
            {
                lua_rawgeti(lua, -1, i);
                switch (lua_type(lua, -1))
                {
                case LUA_TSTRING:
                    custom_command_push_depend(arena, command,
                                               push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1))));
                    break;

                case LUA_TUSERDATA: {
                    Target *target = ud_target_get(lua, -1)->value;
                    custom_command_push_depend(arena, command, push_strf(arena, "%.*s", str_varg(target->name)));
                    break;
                }

                default:
                    return luaL_error(lua, "Expected string or target");
                }

                lua_pop(lua, 1);
            }
        }
        else if (strcmp(key, "output") == 0)
        {
            luaL_checktype(lua, -1, LUA_TTABLE);
            for (int i = 1; i <= lua_rawlen(lua, -1); i++)
            {
                lua_rawgeti(lua, -1, i);
                custom_command_push_output(arena, command,
                                           push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1))));
                lua_pop(lua, 1);
            }
        }
        else if (strcmp(key, "command") == 0)
        {
            command->command = push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1)));
        }
        else if (strcmp(key, "comment") == 0)
        {
            command->comment = push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1)));
        }
        else
        {
            return luaL_error(lua, "Unknown key: %s", key);
        }
    }
    command->thing = api_thing_lists_push_thing(lua, ThingType_CustomCommand, command);
    return 1;
}

// local opt = option("name", "description", "value")
static int api_option(lua_State *lua)
{
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    const char *name = luaL_checkstring(lua, 1);
    const char *description = luaL_checkstring(lua, 2);
    const char *value = luaL_checkstring(lua, 3);
    Option *option = ud_option_new(arena, lua)->value;
    option->name = push_str_copy(arena, str_from_cstr((u8 *)name));
    option->description = push_str_copy(arena, str_from_cstr((u8 *)description));
    option->value = push_str_copy(arena, str_from_cstr((u8 *)value));
    option->thing = api_thing_lists_push_thing(lua, ThingType_Option, option);
    return 1;
}

// when(condition, function() ... end)
static int api_when(lua_State *lua)
{
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    Condition *condition = ud_condition_get_or_make(arena, lua, 1);
    luaL_checktype(lua, 2, LUA_TFUNCTION);
    When *when = ud_when_new(arena, lua)->value;
    when_push_condition(when, condition);

    // Push the current when
    when->thing = api_thing_lists_push_thing(lua, ThingType_When, when);

    // Push the new thing list
    api_thing_lists_push(lua, &when->things);

    // Call the function
    lua_pushvalue(lua, 2);
    lua_call(lua, 0, 0);

    // Pop the thing list
    api_thing_lists_pop(lua);
    return 1;
}

// local cond = condition("MSVC")
// or
// local cond = condition({"MSVC", "WIN32"})
static int api_condition(lua_State *lua)
{
    Arena *arena = api_get_arena(lua);
    switch (lua_type(lua, 1))
    {
    case LUA_TSTRING: {
        const char *expression = luaL_checkstring(lua, 1);
        Condition *condition = ud_condition_new(arena, lua)->value;
        condition_push_expression(arena, condition, push_str_copy(arena, str_from_cstr((u8 *)expression)));
        return 1;
    }
    case LUA_TTABLE: {
        Condition *condition = ud_condition_new(arena, lua)->value;
        for (int i = 1; i <= lua_rawlen(lua, 1); i++)
        {
            lua_rawgeti(lua, 1, i);
            const char *expression = luaL_checkstring(lua, -1);
            condition_push_expression(arena, condition, push_str_copy(arena, str_from_cstr((u8 *)expression)));
            lua_pop(lua, 1);
        }
        return 1;
    }
    default:
        return luaL_error(lua, "Expected string or table");
    }
}

// local files = glob("src/*.c")
static int api_glob(lua_State *lua)
{
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    const char *pattern = luaL_checkstring(lua, 1);
    Glob *glob = ud_glob_new(arena, lua)->value;
    glob->pattern = push_str_copy(arena, str_from_cstr((u8 *)pattern));
    glob->thing = api_thing_lists_push_thing(lua, ThingType_Glob, glob);
    return 1;
}

// package("gh:username/repo@1.0")
// or
// package{
//     name = "package",
//     url = "https://github.com/test/package",
//     tag = "v1.0",
//     options = { "option1", "option2" },
//     download_only = false,
// }
static int api_package(lua_State *lua)
{
    luaL_checktype(lua, 1, LUA_TTABLE);
    Arena *arena = api_get_arena(lua);
    Project *project = api_get_project(lua);
    Package *package = ud_package_new(arena, lua)->value;
    for (lua_pushnil(lua); lua_next(lua, 1); lua_pop(lua, 1))
    {
        const char *key = luaL_checkstring(lua, -2);
        if (strcmp(key, "name") == 0)
        {
            package->name = push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1)));
        }
        else if (strcmp(key, "url") == 0)
        {
            package->url = push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1)));
        }
        else if (strcmp(key, "tag") == 0)
        {
            package->tag = push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1)));
        }
        else if (strcmp(key, "options") == 0)
        {
            luaL_checktype(lua, -1, LUA_TTABLE);
            for (int i = 1; i <= lua_rawlen(lua, -1); i++)
            {
                lua_rawgeti(lua, -1, i);
                str_list_push(arena, &package->options,
                              push_str_copy(arena, str_from_cstr((u8 *)luaL_checkstring(lua, -1))));
                lua_pop(lua, 1);
            }
        }
        else if (strcmp(key, "download_only") == 0)
        {
            package->download_only = lua_toboolean(lua, -1);
        }
        else
        {
            return luaL_error(lua, "Unknown key: %s", key);
        }
    }
    package->thing = api_thing_lists_push_thing(lua, ThingType_Package, package);
    return 1;
}

static void add_global_function(lua_State *lua, const char *name, lua_CFunction func)
{
    lua_pushcfunction(lua, func);
    lua_setglobal(lua, name);
}

void api_init_and_equip(lua_State *lua)
{
    add_global_function(lua, "project", api_project);
    add_global_function(lua, "executable", api_executable);
    add_global_function(lua, "static_library", api_static_library);
    add_global_function(lua, "shared_library", api_shared_library);
    add_global_function(lua, "custom_target", api_custom_target);
    add_global_function(lua, "custom_command", api_custom_command);
    add_global_function(lua, "option", api_option);
    add_global_function(lua, "when", api_when);
    add_global_function(lua, "condition", api_condition);
    add_global_function(lua, "glob", api_glob);
    add_global_function(lua, "package", api_package);

    // Create and add a new arena to the Lua registry.
    Arena *arena = arena_alloc();
    lua_pushlightuserdata(lua, arena);
    lua_setfield(lua, LUA_REGISTRYINDEX, "arena");

    // Equip the user data.
    ud_init_and_equip(lua);
}

void api_release(lua_State *lua)
{
    // Free the arena from the Lua registry.
    lua_getfield(lua, LUA_REGISTRYINDEX, "arena");
    Arena *arena = lua_touserdata(lua, -1);
    arena_release(arena);
}

Project *api_get_project(lua_State *lua)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "project");
    if (lua_isnil(lua, -1))
    {
        luaL_error(lua, "No project has been defined");
        return 0;
    }
    Project *project = ud_project_get(lua, -1)->value;
    lua_pop(lua, 1);
    return project;
}

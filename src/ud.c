#include "lauxlib.h"
#include "ud.h"

//
// Ud
//

static Ud *ud_new(lua_State *lua, Arena *arena, void *data, const char *type)
{
    Ud *ud = lua_newuserdata(lua, sizeof(Ud));
    ud->arena = arena;
    ud->value = data;
    luaL_getmetatable(lua, type);
    lua_setmetatable(lua, -2);
    return ud;
}

//
// Condition
//

Ud *ud_condition_new(Arena *arena, lua_State *lua)
{
    Condition *condition = push_array(arena, Condition, 1);
    return ud_new(lua, arena, condition, "Condition");
}

Ud *ud_condition_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Condition");
}

Ud *ud_condition_test(lua_State *lua, int index)
{
    return luaL_testudata(lua, index, "Condition");
}

Condition *ud_condition_get_or_make(Arena *arena, lua_State *lua, int index)
{
    switch (lua_type(lua, index))
    {
    case LUA_TUSERDATA: {
        Ud *condition_ud = ud_condition_test(lua, index);
        Ud *option_ud = ud_option_test(lua, index);
        if (condition_ud)
        {
            return condition_ud->value;
        }
        else if (option_ud)
        {
            // Make a new condition from the option.
            Option *option = option_ud->value;
            Condition *new_condition = ud_condition_new(arena, lua)->value;
            condition_push_expression(arena, new_condition, push_strf(arena, "${%.*s}", str_varg(option->name)));
            return new_condition;
        }
        else
        {
            luaL_error(lua, "Expected condition or option");
            return 0;
        }
    }

    case LUA_TTABLE: {
        Condition *condition = ud_condition_new(arena, lua)->value;
        for (int i = 1; i <= lua_rawlen(lua, index); i++)
        {
            lua_geti(lua, index, i);
            const char *expression = luaL_checkstring(lua, -1);
            condition_push_expression(arena, condition, push_str_copy(arena, str_from_cstr((u8 *)expression)));
            lua_pop(lua, 1);
        }
        lua_pop(lua, 1);
        return condition;
    }

    default:
        luaL_error(lua, "Expected condition or table");
        return 0;
    }
}

static const luaL_Reg condition_methods[] = {{0, 0}};

//
// Property
//

Ud *ud_property_new(Arena *arena, lua_State *lua)
{
    Property *property = push_array(arena, Property, 1);
    return ud_new(lua, arena, property, "Property");
}

Ud *ud_property_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Property");
}

static int ud_property_when(lua_State *lua)
{
    Ud *ud = ud_property_get(lua, 1);
    Arena *arena = ud->arena;
    Property *property = ud->value;
    Condition *condition = ud_condition_get_or_make(arena, lua, 2);
    property_push_condition(property, condition);
    return 0;
}

static const luaL_Reg property_methods[] = {
    {"when", ud_property_when},
    {0, 0},
};

typedef enum PropertyFromTableFlags PropertyFromTableFlags;
enum PropertyFromTableFlags
{
    PropertyFromTableFlags_None = 0,
    PropertyFromTableFlags_AllowTargets = 1 << 0,
};

// A helper function that creates a property from a table of strings.
static Property *ud_property_from_table(Arena *arena, lua_State *lua, int index, PropertyFromTableFlags flags)
{
    luaL_checktype(lua, index, LUA_TTABLE);
    Property *property = ud_property_new(arena, lua)->value;
    for (int i = 1; i <= lua_rawlen(lua, index); i++)
    {
        lua_geti(lua, index, i);
        if ((flags & PropertyFromTableFlags_AllowTargets) && lua_isuserdata(lua, -1))
        {
            Target *target = ud_target_get(lua, -1)->value;
            property_push_value(arena, property, target->name);
            lua_pop(lua, 1);
        }
        else
        {
            const char *name = luaL_checkstring(lua, -1);
            property_push_value(arena, property, push_str_copy(arena, str_from_cstr((u8 *)name)));
            lua_pop(lua, 1);
        }
    }
    return property;
}

//
// Target
//

Ud *ud_target_new(Arena *arena, lua_State *lua)
{
    Target *target = push_array(arena, Target, 1);
    return ud_new(lua, arena, target, "Target");
}

Ud *ud_target_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Target");
}

static int ud_target_when(lua_State *lua)
{
    Ud *ud = ud_target_get(lua, 1);
    Arena *arena = ud->arena;
    Target *target = ud->value;
    Condition *condition = ud_condition_get_or_make(arena, lua, 2);
    thing_push_condition(target->thing, condition);
    return 0;
}

static int ud_target_public_link_libraries(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_AllowTargets);
    property->type = PropertyType_LinkLibraries;
    property->visibility = PropertyVisibility_Public;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_private_link_libraries(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_AllowTargets);
    property->type = PropertyType_LinkLibraries;
    property->visibility = PropertyVisibility_Private;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_interface_link_libraries(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_AllowTargets);
    property->type = PropertyType_LinkLibraries;
    property->visibility = PropertyVisibility_Interface;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_public_include_directories(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_IncludeDirectories;
    property->visibility = PropertyVisibility_Public;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_private_include_directories(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_IncludeDirectories;
    property->visibility = PropertyVisibility_Private;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_interface_include_directories(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_IncludeDirectories;
    property->visibility = PropertyVisibility_Interface;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_public_compile_definitions(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_CompileDefinitions;
    property->visibility = PropertyVisibility_Public;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_private_compile_definitions(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_CompileDefinitions;
    property->visibility = PropertyVisibility_Private;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_interface_compile_definitions(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_None);
    property->type = PropertyType_CompileDefinitions;
    property->visibility = PropertyVisibility_Interface;
    target_push_property(target->value, property);
    return 1;
}

static int ud_target_dependencies(lua_State *lua)
{
    Ud *target = ud_target_get(lua, 1);
    Property *property = ud_property_from_table(target->arena, lua, 2, PropertyFromTableFlags_AllowTargets);
    property->type = PropertyType_ManuallyAddedDependencies;
    property->visibility = PropertyVisibility_None;
    target_push_property(target->value, property);
    return 1;
}

static const luaL_Reg target_methods[] = {
    {"when", ud_target_when},
    {"public_link_libraries", ud_target_public_link_libraries},
    {"private_link_libraries", ud_target_private_link_libraries},
    {"interface_link_libraries", ud_target_interface_link_libraries},
    {"public_include_directories", ud_target_public_include_directories},
    {"private_include_directories", ud_target_private_include_directories},
    {"interface_include_directories", ud_target_interface_include_directories},
    {"public_compile_definitions", ud_target_public_compile_definitions},
    {"private_compile_definitions", ud_target_private_compile_definitions},
    {"interface_compile_definitions", ud_target_interface_compile_definitions},
    {"dependencies", ud_target_dependencies},
    {0, 0},
};

//
// ListOp
//

Ud *ud_glob_exclusion_new(Arena *arena, lua_State *lua)
{
    ListOp *exclusion = push_array(arena, ListOp, 1);
    return ud_new(lua, arena, exclusion, "ListOp");
}

Ud *ud_glob_exclusion_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "ListOp");
}

static int ud_glob_exclusion_when(lua_State *lua)
{
    Ud *ud = ud_glob_exclusion_get(lua, 1);
    Arena *arena = ud->arena;
    ListOp *exclusion = ud->value;
    Condition *condition = ud_condition_get_or_make(arena, lua, 2);
    list_op_push_condition(exclusion, condition);
    return 0;
}

static const luaL_Reg glob_exclusion_methods[] = {
    {"when", ud_glob_exclusion_when},
    {0, 0},
};

//
// Glob
//

static int glob_count = 0;

Ud *ud_glob_new(Arena *arena, lua_State *lua)
{
    Glob *glob = push_array(arena, Glob, 1);
    // Generate a name for the glob.
    glob->name = push_strf(arena, "glob_%d", glob_count++);
    return ud_new(lua, arena, glob, "Glob");
}

Ud *ud_glob_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Glob");
}

static int ud_glob_exclude(lua_State *lua)
{
    Ud *glob = ud_glob_get(lua, 1);
    const char *pattern = luaL_checkstring(lua, 2);
    ListOp *operation = ud_glob_exclusion_new(glob->arena, lua)->value;
    operation->type = ListOpType_FilterExclude;
    list_op_push_item(glob->arena, operation, push_str_copy(glob->arena, str_from_cstr((u8 *)pattern)));
    glob_push_operation(glob->value, operation);
    return 1;
}

static int ud_glob_remove(lua_State *lua)
{
    Ud *glob = ud_glob_get(lua, 1);
    luaL_checktype(lua, 2, LUA_TTABLE);
    ListOp *operation = ud_glob_exclusion_new(glob->arena, lua)->value;
    operation->type = ListOpType_RemoveItem;
    for (int i = 1; i <= lua_rawlen(lua, 2); i++)
    {
        lua_geti(lua, 2, i);
        const char *pattern = luaL_checkstring(lua, -1);
        list_op_push_item(glob->arena, operation, push_str_copy(glob->arena, str_from_cstr((u8 *)pattern)));
        lua_pop(lua, 1);
    }
    glob_push_operation(glob->value, operation);
    return 1;
}

static const luaL_Reg glob_methods[] = {
    {"exclude", ud_glob_exclude},
    {"remove", ud_glob_remove},
    {0, 0},
};

//
// When
//

Ud *ud_when_new(Arena *arena, lua_State *lua)
{
    When *when = push_array(arena, When, 1);
    return ud_new(lua, arena, when, "When");
}

Ud *ud_when_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "When");
}

static const luaL_Reg when_methods[] = {{0, 0}};

//
// CustomCommand
//

Ud *ud_custom_command_new(Arena *arena, lua_State *lua)
{
    CustomCommand *command = push_array(arena, CustomCommand, 1);
    return ud_new(lua, arena, command, "CustomCommand");
}

Ud *ud_custom_command_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "CustomCommand");
}

static const luaL_Reg custom_command_methods[] = {{0, 0}};

//
// Option
//

Ud *ud_option_new(Arena *arena, lua_State *lua)
{
    Option *option = push_array(arena, Option, 1);
    return ud_new(lua, arena, option, "Option");
}

Ud *ud_option_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Option");
}

Ud *ud_option_test(lua_State *lua, int index)
{
    return luaL_testudata(lua, index, "Option");
}

static const luaL_Reg option_methods[] = {{0, 0}};

//
// Package
//

Ud *ud_package_new(Arena *arena, lua_State *lua)
{
    Package *package = push_array(arena, Package, 1);
    return ud_new(lua, arena, package, "Package");
}

Ud *ud_package_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Package");
}

static int ud_package_target(lua_State *lua)
{
    Ud *package_ud = ud_package_get(lua, 1);
    Arena *arena = package_ud->arena;
    Package *package = package_ud->value;
    const char *name = luaL_checkstring(lua, 2);
    Ud *target_ud = ud_target_new(arena, lua);
    Target *target = target_ud->value;
    target->name = push_str_copy(arena, str_from_cstr((u8 *)name));
    return 1;
}

static const luaL_Reg package_methods[] = {
    {"target", ud_package_target},
    {0, 0},
};

//
// Project
//

Ud *ud_project_new(Arena *arena, lua_State *lua)
{
    Project *project = push_array(arena, Project, 1);
    return ud_new(lua, arena, project, "Project");
}

Ud *ud_project_get(lua_State *lua, int index)
{
    return luaL_checkudata(lua, index, "Project");
}

static const luaL_Reg project_methods[] = {{0, 0}};

//
//
//

static void ud_create_metatable(lua_State *lua, const char *name, const luaL_Reg *methods)
{
    luaL_newmetatable(lua, name);
    luaL_setfuncs(lua, methods, 0);
    lua_pushvalue(lua, -1);
    lua_setfield(lua, -2, "__index");
    lua_pop(lua, 1);
}

void ud_init_and_equip(lua_State *lua)
{
    ud_create_metatable(lua, "Condition", condition_methods);
    ud_create_metatable(lua, "Property", property_methods);
    ud_create_metatable(lua, "Target", target_methods);
    ud_create_metatable(lua, "ListOp", glob_exclusion_methods);
    ud_create_metatable(lua, "Glob", glob_methods);
    ud_create_metatable(lua, "When", when_methods);
    ud_create_metatable(lua, "CustomCommand", custom_command_methods);
    ud_create_metatable(lua, "Option", option_methods);
    ud_create_metatable(lua, "Package", package_methods);
    ud_create_metatable(lua, "Project", project_methods);
}
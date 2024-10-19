#pragma once

#include "utl/utl.h"

typedef struct Condition Condition;
struct Condition
{
    Condition *next;
    StrList expressions;
};

typedef struct ConditionList ConditionList;
struct ConditionList
{
    Condition *head;
    Condition *tail;
    usize count;
};

typedef enum PropertyType PropertyType;
enum PropertyType
{
    PropertyType_LinkLibraries,
    PropertyType_IncludeDirectories,
    PropertyType_CompileDefinitions,
    PropertyType_ManuallyAddedDependencies,
};

typedef enum PropertyVisibility PropertyVisibility;
enum PropertyVisibility
{
    PropertyVisibility_None,
    PropertyVisibility_Public,
    PropertyVisibility_Private,
    PropertyVisibility_Interface,
};

typedef struct Property Property;
struct Property
{
    struct Target *target;
    Property *next;
    PropertyType type;
    PropertyVisibility visibility;
    StrList values;
    ConditionList conditions;
};

typedef struct PropertyList PropertyList;
struct PropertyList
{
    Property *head;
    Property *tail;
    usize count;
};

typedef enum TargetType TargetType;
enum TargetType
{
    TargetType_Executable,
    TargetType_StaticLibrary,
    TargetType_SharedLibrary,
    TargetType_Custom,
};

typedef struct Thing Thing;
typedef struct Target Target;
struct Target
{
    Thing *thing;
    Target *next;
    Str name;
    TargetType type;
    StrList sources;
    PropertyList properties;
};

typedef struct Package Package;
struct Package
{
    Thing *thing;
    Package *next;
    Str name;
    Str url;
    Str tag;
    StrList options;
    bool download_only;
};

typedef enum ListOpType ListOpType;
enum ListOpType
{
    ListOpType_RemoveItem,
    ListOpType_FilterExclude,
};

typedef struct ListOp ListOp;
struct ListOp
{
    ListOp *next;
    ListOpType type;
    StrList items;
    ConditionList conditions;
};

typedef struct ListOpList ListOpList;
struct ListOpList
{
    ListOp *head;
    ListOp *tail;
    usize count;
};

typedef struct Glob Glob;
struct Glob
{
    Thing *thing;
    Glob *next;
    Str name;
    Str pattern;
    bool recursive;
    ListOpList operations;
};

typedef struct ThingList ThingList;
struct ThingList
{
    Thing *head;
    Thing *tail;
    usize count;
};

typedef struct When When;
struct When
{
    Thing *thing;
    When *next;
    ConditionList conditions;
    ThingList things;
};

typedef struct CustomCommand CustomCommand;
struct CustomCommand
{
    Thing *thing;
    CustomCommand *next;
    StrList depends;
    StrList output;
    Str command;
    Str comment;
};

typedef struct Option Option;
struct Option
{
    Thing *thing;
    Option *next;
    Str name;
    Str description;
    Str value;
};

typedef enum ThingType ThingType;
enum ThingType
{
    ThingType_Target,
    ThingType_Package,
    ThingType_Glob,
    ThingType_When,
    ThingType_CustomCommand,
    ThingType_Option,
};

typedef struct Thing Thing;
struct Thing
{
    Thing *next;
    ThingType type;
    union {
        void *data;
        Target *target;
        Package *package;
        Glob *glob;
        When *when;
        CustomCommand *custom_command;
        Option *option;
    };
    ConditionList conditions;
};

typedef struct Project Project;
struct Project
{
    Str name;
    ThingList things;
};

// Condition
void condition_push_expression(Arena *arena, Condition *condition, Str expression);

// ConditionList
void condition_list_push(ConditionList *list, Condition *condition);

// Property
void property_push_value(Arena *arena, Property *property, Str value);
void property_push_condition(Property *property, Condition *condition);

// PropertyList
void property_list_push(PropertyList *list, Property *property);

// Target
void target_push_source(Arena *arena, Target *target, Str source);
void target_push_property(Target *target, Property *property);

// Package
void package_push_option(Arena *arena, Package *package, Str option);

// ListOp
void list_op_push_item(Arena *arena, ListOp *operation, Str item);

// ListOpList
void list_op_list_push(ListOpList *list, ListOp *exclusion);

// Glob
void glob_push_operation(Glob *glob, ListOp *operation);

// CustomCommand
void custom_command_push_depend(Arena *arena, CustomCommand *command, Str depend);
void custom_command_push_output(Arena *arena, CustomCommand *command, Str output);

// Thing
void thing_push_condition(Thing *thing, Condition *condition);

// ThingList
void thing_list_push(ThingList *list, Thing *thing);
Thing *thing_list_push_thing(Arena *arena, ThingList *list, ThingType type, void *thing);

// When
void when_push_condition(When *when, Condition *condition);
void when_push_thing(When *when, Thing *thing);

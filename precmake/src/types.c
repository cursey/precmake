#include "types.h"

//
// Condition
//

void condition_push_expression(Arena *arena, Condition *condition, Str expression)
{
    str_list_push(arena, &condition->expressions, expression);
}

void condition_list_push(ConditionList *list, Condition *condition)
{
    queue_push(list->head, list->tail, condition);
    list->count++;
}

//
// Property
//

void property_push_value(Arena *arena, Property *property, Str value)
{
    str_list_push(arena, &property->values, value);
}

void property_push_condition(Property *property, Condition *condition)
{
    condition_list_push(&property->conditions, condition);
}

//
// PropertyList
//

void property_list_push(PropertyList *list, Property *property)
{
    queue_push(list->head, list->tail, property);
    list->count++;
}

//
// Target
//

void target_push_source(Arena *arena, Target *target, Str source)
{
    str_list_push(arena, &target->sources, source);
}

void target_push_property(Target *target, Property *property)
{
    property_list_push(&target->properties, property);
    property->target = target;
}

//
// Package
//

void package_push_option(Arena *arena, Package *package, Str option)
{
    str_list_push(arena, &package->options, option);
}

//
// ListOp
//

void list_op_push_item(Arena *arena, ListOp *operation, Str item)
{
    str_list_push(arena, &operation->items, item);
}

void list_op_push_condition(ListOp *exclusion, Condition *condition)
{
    condition_list_push(&exclusion->conditions, condition);
}

//
// ListOpList
//

void list_op_list_push(ListOpList *list, ListOp *exclusion)
{
    queue_push(list->head, list->tail, exclusion);
    list->count++;
}

//
// Glob
//

void glob_push_operation(Glob *glob, ListOp *operation)
{
    list_op_list_push(&glob->operations, operation);
}

//
// CustomCommand
//

void custom_command_push_depend(Arena *arena, CustomCommand *command, Str depend)
{
    str_list_push(arena, &command->depends, depend);
}

void custom_command_push_output(Arena *arena, CustomCommand *command, Str output)
{
    str_list_push(arena, &command->output, output);
}

//
// Thing
//

void thing_push_condition(Thing *thing, Condition *condition)
{
    condition_list_push(&thing->conditions, condition);
}

//
// ThingList
//

void thing_list_push(ThingList *list, Thing *thing)
{
    queue_push(list->head, list->tail, thing);
    list->count++;
}

Thing *thing_list_push_thing(Arena *arena, ThingList *list, ThingType type, void *thing)
{
    Thing *new_thing = push_array(arena, Thing, 1);
    new_thing->type = type;
    new_thing->data = thing;
    thing_list_push(list, new_thing);
    return new_thing;
}

//
// When
//

void when_push_condition(When *when, Condition *condition)
{
    condition_list_push(&when->conditions, condition);
}

void when_push_thing(When *when, Thing *thing)
{
    thing_list_push(&when->things, thing);
}

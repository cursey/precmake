project('precmake')

-- Dependency: Lua
local lua_package = package{
    name = 'lua',
    url = 'https://github.com/lua/lua.git',
    tag = 'v5.4.7',
    download_only = true,
}

local lua_sources = glob('${lua_SOURCE_DIR}/*.c')
lua_sources:remove{'${lua_SOURCE_DIR}/lua.c', '${lua_SOURCE_DIR}/luac.c', '${lua_SOURCE_DIR}/onelua.c'}
local lua = static_library('lua', {lua_sources})
lua:public_include_directories{'$<BUILD_INTERFACE:${lua_SOURCE_DIR}>'}

-- utl
local utl_sources = glob('utl/src/*.c')
utl_sources:exclude('.*windows.c$'):when{'NOT WIN32'}
utl_sources:exclude('.*posix.c$'):when{'NOT UNIX'}
local utl = static_library('utl', {utl_sources})
utl:public_include_directories{'utl/include'}

-- precmake
local precmake_sources = glob('precmake/src/*.c')
local precmake = executable('precmake', {precmake_sources})
precmake:private_link_libraries{utl, lua}

